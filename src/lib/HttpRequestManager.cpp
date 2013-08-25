#include <functional>

#include <boost/format.hpp>

#include "utils.hpp"
#include "watcher.hpp"
#include "HttpRequestManager.hpp"

using namespace std::placeholders;

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

    void get(const std::string &url, const HttpRequestManager::Callback &callback) const {
        ConnectionInfo *info = new ConnectionInfo(url, callback);

        curl_easy_setopt(info->easy, CURLOPT_URL, info->url.c_str());
        curl_easy_setopt(info->easy, CURLOPT_NOSIGNAL, 1L);

        struct curl_slist *headers = nullptr;
        curl_easy_setopt(info->easy, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(info->easy, CURLOPT_HEADERFUNCTION, HttpRequestManagerImpl::headerCallback);
        curl_easy_setopt(info->easy, CURLOPT_HEADERDATA, info);
        curl_easy_setopt(info->easy, CURLOPT_WRITEFUNCTION, HttpRequestManagerImpl::writeCallback);
        curl_easy_setopt(info->easy, CURLOPT_WRITEDATA, info);

        curl_easy_setopt(info->easy, CURLOPT_PRIVATE, info);

        CURLMcode code = curl_multi_add_handle(multi, info->easy);
        if (code != CURLM_OK) {
            callback(HttpReply());
        }
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
                ConnectionInfo *info = nullptr;
                char *effectiveUrl = nullptr;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &info);
                curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
                LOG_DEBUG("Done! Code: %d", code);
                info->reply.body = info->data.str();
                info->callback(std::move(info->reply));
                curl_multi_remove_handle(multi, easy);
                delete info;
            }
        }
    }

    //! [CURLMOPT_SOCKETFUNCTION]
    static int onMultiSocketCallback(CURL *easy, curl_socket_t sockfd, int action, HttpRequestManagerImpl *manager, Watcher<T> *io) {
        LOG_DEBUG("fd: %d, event: %d", sockfd, action);

        if (action == CURL_POLL_REMOVE) {
            unwatchSocket(io, manager);
        } else {
            if (!io) {
                watchSocket(sockfd, io, action, manager);
            } else {
                updateSocket(sockfd, io, action, manager);
            }
        }

        return 0;
    }

    static void watchSocket(curl_socket_t sockfd, Watcher<T> *io, int action, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d", sockfd);
        io = new Watcher<T>(manager->loop, std::bind(&HttpRequestManagerImpl::onEvent, manager, _1, _2));
        curl_multi_assign(manager->multi, sockfd, io);
        updateSocket(sockfd, io, action, manager);
    }

    static void unwatchSocket(Watcher<T> *io, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d", io->fd());
        if (io) {
            delete io;
        }
    }

    static void updateSocket(curl_socket_t sockfd, Watcher<T> *io, int events, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d, action: %d", sockfd, events);
        io->process_events(sockfd, events);
    }

    //! [CURLMOPT_TIMERFUNCTION]
    static int onMultiTimerCallback(CURLM *multi, long timeout, HttpRequestManagerImpl *manager) {
        manager->timer.restart(timeout);
        return 0;
    }

    //! [CURLOPT_WRITEFUNCTION]
    static size_t writeCallback(char *data, size_t size, size_t nmemb, ConnectionInfo *info) {
        LOG_DEBUG("size: %d, nmemb: %d, url %s", size, nmemb, info->url);
        const size_t written = size * nmemb;
        info->data.write(data, written);
        return written;
    }

    //! [CURLOPT_HEADERFUNCTION]
    static size_t headerCallback(char *data, size_t size, size_t nmemb, ConnectionInfo *info) {
        LOG_DEBUG("size: %d, nmemb: %d", size, nmemb);
        const size_t written = size * nmemb;
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
void HttpRequestManager<T>::get(const std::string &url, const HttpRequestManager::Callback &callback) const
{
    d->get(url, callback);
}

template class HttpRequestManager<EvProvider>;
