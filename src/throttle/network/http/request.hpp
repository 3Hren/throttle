#pragma once

#include <string>
#include <unordered_map>

#include <boost/lexical_cast.hpp>

#include "header.hpp"

namespace throttle { namespace network { namespace http {

class NetworkRequest {
    std::string url;
    HeaderList headerList;

public:
    NetworkRequest();
    NetworkRequest(const char *url);

    std::string getUrl() const;
    void setUrl(const std::string &url);

    HeaderList &headers();
    const HeaderList &headers() const;
};

} } }
