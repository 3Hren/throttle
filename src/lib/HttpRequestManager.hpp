#pragma once

#include <curl/multi.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>

struct HttpReply {
    std::string body;
};

struct ConnectionInfo {
    typedef std::function<void(const HttpReply&)> Callback;

    CURL *easy;

    const std::string url;
    Callback callback;

    std::stringstream data;
    HttpReply reply;

    ConnectionInfo(const std::string &url, const Callback &callback) :
        easy(curl_easy_init()), //! @warning: may fail.
        url(url),
        callback(callback)
    {}

    ~ConnectionInfo() {
        curl_easy_cleanup(easy);
    }
};

class HttpRequestManager {
    class HttpRequestManagerImpl;
    std::unique_ptr<HttpRequestManagerImpl> d;
public:
    typedef std::function<void(const HttpReply&)> Callback;

    HttpRequestManager(boost::asio::io_service &io_service);
    ~HttpRequestManager();

    void get(const std::string &url, const Callback &callback) const;
};
