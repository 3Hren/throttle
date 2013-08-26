#include <functional>

#include "utils.hpp"
#include "event/watcher.hpp"
#include "request.hpp"
#include "manager.hpp"
#include "connection/info.hpp"

using namespace std::placeholders;

using namespace throttle::event;
using namespace throttle::network::http;

namespace helpers {

std::string trimmed(const char *begin, const char *end) {
    while (begin < end && isspace(*begin))
        ++begin;
    while (begin < end && isspace(*(end - 1)))
        --end;
    return std::string(begin, end);
}

}

template<typename T>
class HttpRequestManager<T>::HttpRequestManagerImpl {
public:
    typedef typename T::Loop Loop;
    typedef typename T::Timer Timer;

    Loop &loop;
    Timer timer;
    CURLM *multi;

    int still_running;

    HttpRequestManagerImpl(T &provider) :
        loop(provider.loop),
        timer(provider.loop, std::bind(&HttpRequestManagerImpl::onTimer, this, _1)),
        multi(curl_multi_init()),
        still_running(0)
    {
        curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, onMultiSocketCallback);
        curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this);
        curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, onMultiTimerCallback);
        curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this);
    }

    ~HttpRequestManagerImpl() {
        curl_multi_cleanup(multi);
    }

    void get(const NetworkRequest &request, const Callbacks &callbacks) const {
        connection::Info *info = new connection::Info(request, callbacks);

        curl_easy_setopt(info->easy, CURLOPT_URL, request.getUrl().c_str());
        curl_easy_setopt(info->easy, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(info->easy, CURLOPT_NOPROGRESS, 1L);

        struct curl_slist *rawHeaders = packHeaders(request.getHeaders());
        curl_easy_setopt(info->easy, CURLOPT_HTTPHEADER, rawHeaders);

        curl_easy_setopt(info->easy, CURLOPT_HEADERFUNCTION, HttpRequestManagerImpl::headerCallback);
        curl_easy_setopt(info->easy, CURLOPT_HEADERDATA, info);
        curl_easy_setopt(info->easy, CURLOPT_WRITEFUNCTION, HttpRequestManagerImpl::writeCallback);
        curl_easy_setopt(info->easy, CURLOPT_WRITEDATA, info);

        curl_easy_setopt(info->easy, CURLOPT_PRIVATE, info);

        CURLMcode code = curl_multi_add_handle(multi, info->easy);
        if (code != CURLM_OK) {
            callbacks.onFinished(HttpReply());
        }
    }

private:
    struct curl_slist *packHeaders(const std::unordered_map<std::string, std::string> &headers) const {
        struct curl_slist *rawHeaders = nullptr;
        for (auto it = headers.begin(); it != headers.end(); ++it) {
            const std::string &name = it->first;
            const std::string &value = it->second;
            std::string rawHeader;
            rawHeader.reserve(name.size() + 1 + value.size());
            rawHeader += name;
            rawHeader += ":";
            rawHeader += value;
            rawHeaders = curl_slist_append(rawHeaders, rawHeader.c_str());
        }
        return rawHeaders;
    }

    void onEvent(int sockfd, int events) {
        LOG_DEBUG("action: %d", events);
        CURLMcode code = curl_multi_socket_action(multi, sockfd, events, &still_running);
        LOG_DEBUG("CURLMcode: %d, still_running: %d, fd: %d", code, still_running, sockfd);
        BOOST_ASSERT(code != CURLM_CALL_MULTI_PERFORM);

        checkCompleted();
        if (still_running <= 0) {
            timer.stop();
        }
    }

    void onTimer(int ec) {
        LOG_DEBUG("error code: %d", ec);
        CURLMcode rc = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &still_running);
        LOG_DEBUG("CURLMcode: %d, still_running: %d", rc, still_running);
        BOOST_ASSERT(rc != CURLM_CALL_MULTI_PERFORM);

        checkCompleted();
    }

    void checkCompleted() {
        LOG_DEBUG("Enter, remaining: %d", still_running);

        int messagesLeft = 0;
        while (true) {
            CURLMsg *message = curl_multi_info_read(multi, &messagesLeft);
            LOG_DEBUG("Loop iteration, messagesLeft: %d, message: %s", messagesLeft, message);
            if (!message)
                break;

            LOG_DEBUG("message: %d", message->msg);
            if (message->msg == CURLMSG_DONE) {
                CURL *easy = message->easy_handle;
                CURLcode code = message->data.result;
                connection::Info *info = nullptr;
                char *effectiveUrl = nullptr;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &info);
                curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
                LOG_DEBUG("Done! Code: %d", code);
                info->reply.body = info->data.str();
                info->callbacks.onFinished(std::move(info->reply));
                curl_multi_remove_handle(multi, easy);
                delete info;
            }
        }
    }

    //! [CURLMOPT_SOCKETFUNCTION]
    static int onMultiSocketCallback(CURL *easy, curl_socket_t sockfd, int action, HttpRequestManagerImpl *manager, Watcher<T> *watcher) {
        LOG_DEBUG("fd: %d, event: %d", sockfd, action);

        if (action == CURL_POLL_REMOVE) {
            unwatchSocket(watcher, manager);
        } else {
            if (!watcher) {
                watchSocket(sockfd, watcher, action, manager);
            } else {
                updateSocket(sockfd, watcher, action, manager);
            }
        }

        return 0;
    }

    static void watchSocket(curl_socket_t sockfd, Watcher<T> *watcher, int action, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d", sockfd);
        watcher = new Watcher<T>(manager->loop, std::bind(&HttpRequestManagerImpl::onEvent, manager, _1, _2));
        curl_multi_assign(manager->multi, sockfd, watcher);
        updateSocket(sockfd, watcher, action, manager);
    }

    static void unwatchSocket(Watcher<T> *watcher, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d", watcher->fd());
        if (watcher) {
            delete watcher;
        }
    }

    static void updateSocket(curl_socket_t sockfd, Watcher<T> *watcher, int events, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d, action: %d", sockfd, events);
        watcher->process_events(sockfd, events);
    }

    //! [CURLMOPT_TIMERFUNCTION]
    static int onMultiTimerCallback(CURLM *multi, long timeout, HttpRequestManagerImpl *manager) {
        manager->timer.restart(timeout);
        return 0;
    }

    //! [CURLOPT_WRITEFUNCTION]
    static size_t writeCallback(char *data, size_t size, size_t nmemb, connection::Info *info) {
        LOG_DEBUG("size: %d, nmemb: %d", size, nmemb);
        const size_t written = size * nmemb;
        info->data.write(data, written);
        if (info->callbacks.onBody) {
            info->callbacks.onBody(data, written);
        }
        return written;
    }

    //! [CURLOPT_HEADERFUNCTION]
    static size_t headerCallback(char *data, size_t size, size_t nmemb, connection::Info *info) {
        LOG_DEBUG("size: %d, nmemb: %d", size, nmemb);
        const size_t written = size * nmemb;

        char *lf;
        char *end = data + written;
        char *colon = std::find(data, end, ':');

        if (colon != end) {
            const std::string field = helpers::trimmed(data, colon);
            std::string value;

            data = colon + 1;
            do {
                lf = std::find(data, end, '\n');
                if (!value.empty())
                    value += ' ';

                value += helpers::trimmed(data, lf);
                data = lf;
            } while (data < end && (*(data + 1) == ' ' || *(data + 1) == '\t'));

            info->reply.setHeader({field, value});
        }

        return written;
    }
};

template<typename T>
HttpRequestManager<T>::HttpRequestManager(T &provider) :
    d(new HttpRequestManagerImpl(provider))
{
}

template<typename T>
HttpRequestManager<T>::~HttpRequestManager()
{
}

template<typename T>
void HttpRequestManager<T>::get(const NetworkRequest &request, const Callbacks::OnFinished &callback) const
{
    get(request, Callbacks(callback));
}

template<typename T>
void HttpRequestManager<T>::get(const NetworkRequest &request, const Callbacks &callbacks) const
{
    d->get(request, callbacks);
}

#include "event/provider/ev.hpp"
template class throttle::network::http::HttpRequestManager<throttle::event::provider::Ev>;
