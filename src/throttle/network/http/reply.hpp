#pragma once

#include <string>
#include <list>

#include "header.hpp"

namespace throttle { namespace network { namespace http {

struct NetworkReply {
    std::list<Header> headers;
    std::string body;

    NetworkReply() = default;
    NetworkReply(const NetworkReply &other) = default;
    NetworkReply(NetworkReply &&other);

    bool hasHeader(const std::string &name) const;
    std::string getHeader(const std::string &name) const;
    void setHeader(const Header &header);
    void setHeader(Header &&header);
};

} } }
