#pragma once

#include <vector>

#include <curl/multi.h>
#include <ev++.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "timer.hpp"

struct AsioProvider {
    typedef typename boost::asio::io_service Loop;
    typedef typename boost::asio::deadline_timer Timer;
};

struct EvProvider {
    typedef typename ev::loop_ref Loop;
    typedef typename ::Timer<EvProvider> Timer;

    Loop &loop;
};

struct Header {
    std::string name;
    std::string value;
};

struct HttpReply {
    std::vector<Header> headers;
    std::string body;

    HttpReply() = default;
    HttpReply(HttpReply &&other) :
        headers(std::move(other.headers)),
        body(std::move(other.body))
    {}
};

struct ConnectionInfo {
    typedef std::function<void(HttpReply&&)> Callback;

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

template<typename T>
class HttpRequestManager {
    class HttpRequestManagerImpl;
    std::unique_ptr<HttpRequestManagerImpl> d;
public:
    typedef std::function<void(HttpReply&&)> Callback;

    HttpRequestManager(T &provider);
    ~HttpRequestManager();

    void get(const std::string &url, const Callback &callback) const;
};
