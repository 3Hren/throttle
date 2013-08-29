#pragma once

#include <string>
#include <list>

#include "header.hpp"
#include "request.hpp"

namespace throttle { namespace network { namespace http {

class NetworkReply {
    NetworkRequest request;
    HeaderList headerList;
    std::string body;

public:
    NetworkReply() = default;
    NetworkReply(const NetworkReply &other) = default;
    NetworkReply(NetworkReply &&other);

    void setRequest(NetworkRequest &&request);

    HeaderList &headers();
    const HeaderList &headers() const;

    std::string getBody() const;
    void setBody(const std::string &body);
};

} } }
