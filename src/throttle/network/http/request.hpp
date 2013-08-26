#pragma once

#include <string>
#include <unordered_map>

namespace throttle { namespace network { namespace http {

class NetworkRequest {
    std::string url;
    std::unordered_map<std::string, std::string> headers;

public:
    NetworkRequest();
    NetworkRequest(const char *url);

    std::string getUrl() const;
    void setUrl(const std::string &url);

    bool hasHeader(const std::string &name) const;
    std::string getHeader(const std::string &name) const;
    void setHeader(const std::string &name, const std::string &value);

    std::unordered_map<std::string, std::string> getHeaders() const;
};

} } }
