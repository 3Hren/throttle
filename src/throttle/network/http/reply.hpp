#pragma once

#include <string>
#include <list>

#include "header.hpp"

namespace throttle { namespace network { namespace http {

class NetworkReply {
public:
    HeaderList headerList;
    std::string body;

    NetworkReply() = default;
    NetworkReply(const NetworkReply &other) = default;
    NetworkReply(NetworkReply &&other);

    HeaderList &headers();
    const HeaderList &headers() const;
};

} } }
