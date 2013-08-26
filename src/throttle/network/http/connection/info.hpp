#pragma once

#include <sstream>

#include <curl/curl.h>

#include "network/http/request.hpp"
#include "network/http/reply.hpp"
#include "network/http/callbacks.hpp"

namespace throttle { namespace network { namespace http { namespace connection {

struct Info {
    CURL *easy;

    const NetworkRequest request;
    const Callbacks callbacks;

    std::stringstream data;
    HttpReply reply;

    Info(const NetworkRequest &request, const Callbacks &callbacks);
    ~Info();
};

} } } }
