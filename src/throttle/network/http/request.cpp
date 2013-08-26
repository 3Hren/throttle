#include "request.hpp"

using namespace throttle::network::http;

NetworkRequest::NetworkRequest() {
}

NetworkRequest::NetworkRequest(const char *url) :
    url(url)
{}

std::string NetworkRequest::getUrl() const {
    return url;
}

void NetworkRequest::setUrl(const std::string &url) {
    this->url = url;
}

bool NetworkRequest::hasHeader(const std::string &name) const {
    return headers.find(name) != headers.end();
}

std::string NetworkRequest::getHeader(const std::string &name) const {
    auto it = headers.find(name);
    if (it != headers.end()) {
        return it->second;
    }
    return std::string();
}

void NetworkRequest::setHeader(const std::string &name, const std::string &value) {
    headers.emplace(name, value);
}

std::unordered_map<std::string, std::string> NetworkRequest::getHeaders() const {
    return headers;
}
