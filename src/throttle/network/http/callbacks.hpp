#pragma once

#include <functional>

#include "header.hpp"

namespace throttle { namespace network { namespace http {

struct HttpReply;
struct Callbacks {
    typedef std::function<void(const Header &header)> OnHeader;
    typedef std::function<void(const char *body, const size_t size)> OnBody;
    typedef std::function<void(HttpReply&&)> OnFinished;

    OnHeader onHeader;
    OnBody onBody;
    OnFinished onFinished;

    Callbacks(const OnFinished &onFinished) :
        onFinished(onFinished)
    {}
};

} } }
