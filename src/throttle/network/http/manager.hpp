#pragma once

#include <list>
#include <algorithm>

#include <curl/multi.h>
#include <ev++.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "event/timer.hpp"
#include "callbacks.hpp"
#include "reply.hpp"

namespace throttle { namespace network { namespace http {

class NetworkRequest;

template<typename T>
class HttpRequestManager {
    class HttpRequestManagerImpl;
    std::unique_ptr<HttpRequestManagerImpl> d;
public:
    typedef std::function<void(HttpReply&&)> Callback;

    HttpRequestManager(T &provider);
    ~HttpRequestManager();

    void get(const NetworkRequest &request, const Callbacks::OnFinished &callback) const;
    void get(const NetworkRequest &request, const Callbacks &callbacks) const;
};

} } }
