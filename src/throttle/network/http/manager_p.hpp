#pragma once

#include <string>
#include <mutex>

#include <curl/curl.h>

#include "utils.hpp"
#include "event/timer.hpp"
#include "event/watcher.hpp"
#include "connection/info.hpp"

using namespace std::placeholders;

using namespace throttle::event;

namespace throttle { namespace network { namespace http {

struct Get {
    const NetworkRequest &request;
    const Callbacks &callbacks;

    void operator ()(connection::Info *info) {
        UNUSED(info);
    }

private:
    DECLARE_NONCOPYABLE(Get)
};

struct Post {
    const NetworkRequest &request;
    const std::string &data;
    const Callbacks &callbacks;

    void operator ()(connection::Info *info) {
        info->data = data;
        curl_easy_setopt(info->easy, CURLOPT_POSTFIELDS, info->data.c_str());
    }

private:
    DECLARE_NONCOPYABLE(Post)
};

struct Delete {
    const NetworkRequest &request;
    const Callbacks &callbacks;

    void operator ()(connection::Info *info) {
        curl_easy_setopt(info->easy, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

private:
    DECLARE_NONCOPYABLE(Delete)
};

template<typename T>
class HttpRequestManager<T>::HttpRequestManagerImpl {
    DECLARE_NONCOPYABLE(HttpRequestManagerImpl)

    typedef typename T::Loop Loop;
    typedef typename T::Timer Timer;

    Loop &loop;
    Timer timer;
    CURLM *multi;

    int still_running;
public:
    HttpRequestManagerImpl(T &provider) :
        loop(provider.loop),
        timer(provider.loop, std::bind(&HttpRequestManagerImpl::onTimer, this, _1)),
        multi(curl_multi_init()),
        still_running(0)
    {
        curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, HttpRequestManagerImpl::onMultiSocketCallback);
        curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, this);
        curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, HttpRequestManagerImpl::onMultiTimerCallback);
        curl_multi_setopt(multi, CURLMOPT_TIMERDATA, this);
    }

    ~HttpRequestManagerImpl() {
        curl_multi_cleanup(multi);
    }

    void get(const NetworkRequest &request, const Callbacks &callbacks) const {
        Get handler{ request, callbacks };
        doRequest(handler);
    }

    void post(const NetworkRequest &request, const std::string &data, const Callbacks &callbacks) const {
        Post handler{ request, data, callbacks };
        doRequest(handler);
    }

    void deleteResource(const NetworkRequest &request, const Callbacks &callbacks) const {
        Delete handler{ request, callbacks };
        doRequest(handler);
    }

private:
    template<typename Action>
    void doRequest(Action&& action) const {
        connection::Info *info = new connection::Info(action.request, action.callbacks);
        action(info);
        info->send(multi);
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
                info->reply.body = info->bodyStream.str();
                if (info->callbacks.onFinished) {
                    info->callbacks.onFinished(std::move(info->reply));
                }
                curl_multi_remove_handle(multi, easy);
                delete info;
            }
        }
    }

    //! [CURLMOPT_SOCKETFUNCTION]
    static int onMultiSocketCallback(CURL *easy, curl_socket_t sockfd, int action, HttpRequestManagerImpl *manager, Watcher<T> *watcher) {
        LOG_DEBUG("fd: %d, event: %d", sockfd, action);

        if (action == CURL_POLL_REMOVE) {
            unwatchSocket(watcher);
        } else {
            if (!watcher) {
                watchSocket(sockfd, watcher, action, manager);
            } else {
                updateSocket(sockfd, watcher, action);
            }
        }

        return 0;
    }

    static void watchSocket(curl_socket_t sockfd, Watcher<T> *watcher, int action, HttpRequestManagerImpl *manager) {
        LOG_DEBUG("fd: %d", sockfd);
        watcher = new Watcher<T>(manager->loop, std::bind(&HttpRequestManagerImpl::onEvent, manager, _1, _2));
        curl_multi_assign(manager->multi, sockfd, watcher);
        updateSocket(sockfd, watcher, action);
    }

    static void unwatchSocket(Watcher<T> *watcher) {
        LOG_DEBUG("fd: %d", watcher->fd());
        if (watcher) {
            delete watcher;
            watcher = nullptr;
        }
    }

    static void updateSocket(curl_socket_t sockfd, Watcher<T> *watcher, int events) {
        LOG_DEBUG("fd: %d, action: %d", sockfd, events);
        watcher->process_events(sockfd, events);
    }

    //! [CURLMOPT_TIMERFUNCTION]
    static int onMultiTimerCallback(CURLM *multi, long timeout, HttpRequestManagerImpl *manager) {
        manager->timer.restart(timeout);
        return 0;
    }
};

} } }
