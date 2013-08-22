#include <functional>
#include <unordered_map>

#include <boost/format.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "HttpRequestManager.hpp"

using namespace std::placeholders;
using namespace boost;

typedef boost::asio::io_service IOService;

class HttpRequestManager::HttpRequestManagerImpl {
public:
    IOService &io_service;
    boost::asio::deadline_timer timer;
    CURLM *multi;

    int still_running;
    std::unordered_map<curl_socket_t, boost::asio::ip::tcp::socket *> sockets;

    HttpRequestManagerImpl(IOService &io_service) :
        io_service(io_service),
        timer(io_service),
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
        curl_easy_setopt(info->easy, CURLOPT_WRITEFUNCTION, HttpRequestManagerImpl::writeCallback);
        curl_easy_setopt(info->easy, CURLOPT_WRITEDATA, info);

        curl_easy_setopt(info->easy, CURLOPT_OPENSOCKETFUNCTION, HttpRequestManagerImpl::openSocketCallback);
        curl_easy_setopt(info->easy, CURLOPT_OPENSOCKETDATA, this);

        curl_easy_setopt(info->easy, CURLOPT_CLOSESOCKETFUNCTION, HttpRequestManagerImpl::closeSocketCallback);
        curl_easy_setopt(info->easy, CURLOPT_CLOSESOCKETDATA, this);

        curl_easy_setopt(info->easy, CURLOPT_PRIVATE, info);

        CURLMcode code = curl_multi_add_handle(multi, info->easy);
        if (code == CURLM_OK) {
        } else {
            callback(HttpReply());
        }
    }

private:
    void onEvent(boost::asio::ip::tcp::socket *socket, int action) {
        std::cout << format("--- %s:: action: %d") % __func__ % action << std::endl;
        CURLMcode code = curl_multi_socket_action(multi, socket->native_handle(), action, &still_running);
        BOOST_ASSERT(code != CURLM_CALL_MULTI_PERFORM);

        checkCompleted();
        if (still_running <= 0) {
            timer.cancel();
        }
    }

    void timerCallback(const boost::system::error_code &ec) {
        std::cout << format("--- %s:: error code: %d") % __func__ % ec << std::endl;
        if (!ec) {
            std::cout << format("--- %s:: error!") % __func__ << std::endl;
            //!@todo: maybe loop it in do/while?
            CURLMcode rc = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &still_running);
            std::cout << format("--- %s:: error! CURLMcode: %d") % __func__ % rc << std::endl;
            BOOST_ASSERT(rc != CURLM_CALL_MULTI_PERFORM);

            checkCompleted();
        }
    }

    void checkCompleted() {
        std::cout << format("--- %s::") % __func__ << std::endl;
        int messagesLeft = 0;

        while (CURLMsg *message = curl_multi_info_read(multi, &messagesLeft)) {
            if (message->msg == CURLMSG_DONE) {
                CURL *easy = message->easy_handle;
                CURLcode code = message->data.result;

                ConnectionInfo *info = nullptr;
                char *effectiveUrl = nullptr;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &info);
                curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &effectiveUrl);
                std::cout << format("--- %s:: Done! Code: %d") % __func__ % code << std::endl;
                info->reply.body = info->data.str();
                info->callback(info->reply);
                curl_multi_remove_handle(multi, easy);
                delete info;
            }
        }
    }

    //! --- STATICS ---

    static int onMultiSocketCallback(CURL *easy, curl_socket_t sockfd, int action, HttpRequestManagerImpl *manager, void *sock_ptr) {
        std::cout << boost::format("--- %s:: fd: %d, event: %d") % __func__ % sockfd % action << std::endl;
        int *actionp = reinterpret_cast<int*>(sock_ptr);

        if (action == CURL_POLL_REMOVE) {
            unwatchSocket(actionp, manager);
        } else {
            if (!actionp) {
                watchSocket(sockfd, action, manager);
            } else {
                updateSocket(sockfd, action, actionp, manager);
            }
        }

        return 0;
    }

    static void watchSocket(curl_socket_t sockfd, int action, HttpRequestManagerImpl *manager) {
        std::cout << format("--- %s:: fd: %d") % __func__ % sockfd << std::endl;
        int *actionp = new int;
        updateSocket(sockfd, action, actionp, manager);
        curl_multi_assign(manager->multi, sockfd, actionp);
    }

    static void unwatchSocket(int *actionp, HttpRequestManagerImpl *manager) {
        std::cout << format("--- %s::") % __func__ << std::endl;
        if (actionp) {
            delete actionp;
        }
    }

    static void updateSocket(curl_socket_t sockfd, int action, int *actionp, HttpRequestManagerImpl *manager) {
        std::cout << format("--- %s:: fd: %d, action: %d") % __func__ % sockfd % action << std::endl;

        auto it = manager->sockets.find(sockfd);
        if (it == manager->sockets.end()) {
            BOOST_ASSERT(false);
            return;
        }

        boost::asio::ip::tcp::socket *socket = it->second;
        *actionp = action;

        auto callback = std::bind(&HttpRequestManagerImpl::onEvent, manager, socket, action);
        if (action == CURL_POLL_IN) {
            socket->async_read_some(boost::asio::null_buffers(), callback);
        } else if (action == CURL_POLL_OUT) {
            socket->async_write_some(boost::asio::null_buffers(), callback);
        } else if (action == CURL_POLL_INOUT) {
            socket->async_read_some(boost::asio::null_buffers(), callback);
            socket->async_write_some(boost::asio::null_buffers(), callback);
        }
    }

    static int onMultiTimerCallback(CURLM *multi, long timeout, HttpRequestManagerImpl *manager) {
        std::cout << format("--- %s:: timeout: %d") % __func__ % timeout << std::endl;
        boost::system::error_code ec;
        manager->timer.cancel(ec);
        //!@todo: Check ec.

        if (timeout > 0) {
            manager->timer.expires_from_now(boost::posix_time::milliseconds(timeout), ec);
            //!@todo: Check ec.
            manager->timer.async_wait(std::bind(&HttpRequestManagerImpl::timerCallback, manager, _1));
        } else {
            boost::system::error_code ec;
            manager->timerCallback(ec);
        }
        return 0;
    }

    static curl_socket_t openSocketCallback(HttpRequestManagerImpl *manager, curlsocktype purpose, struct curl_sockaddr *address) {
        std::cout << format("--- %s:: family: %d, protocol: %d, socktype: %d") % __func__ % address->family % address->protocol % address->socktype << std::endl;

        if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET) {
            boost::asio::ip::tcp::socket *socket = new boost::asio::ip::tcp::socket(manager->io_service);
            boost::system::error_code ec;
            socket->open(boost::asio::ip::tcp::v4(), ec);
            if (ec) {
                std::cout << format("--- %s:: open socket error: %d") % __func__ % ec << std::endl;
                return CURL_SOCKET_BAD;
            } else {
                curl_socket_t sockfd = socket->native_handle();
                manager->sockets.insert({sockfd, socket});
                return sockfd;
            }
        }

        return CURL_SOCKET_BAD;
    }

    static int closeSocketCallback(HttpRequestManagerImpl *manager, curl_socket_t sockfd) {
        std::cout << format("--- %s:: fd: %d") % __func__ % sockfd << std::endl;

        //!@todo: lock
        auto it = manager->sockets.find(sockfd);
        if (it != manager->sockets.end()) {
            delete it->second;
            manager->sockets.erase(it);
            return 0;
        }
        return 1;
    }

    //! [CURLOPT_WRITEFUNCTION]
    static size_t writeCallback(char *data, size_t size, size_t nmemb, ConnectionInfo *info) {
        std::cout << format("--- %s:: size: %d, nmemb: %d, url %s") % __func__ % size % nmemb % info->url << std::endl;
        const size_t written = size * nmemb;
        info->data.write(data, written);
        return written;
    }
};

HttpRequestManager::HttpRequestManager(boost::asio::io_service &io_service) :
    d(new HttpRequestManagerImpl(io_service))
{
}

HttpRequestManager::~HttpRequestManager()
{
}

void HttpRequestManager::get(const std::string &url, const HttpRequestManager::Callback &callback) const
{
    d->get(url, callback);
}
