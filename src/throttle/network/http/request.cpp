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

HeaderList &NetworkRequest::headers() {
    return headerList;
}

const HeaderList &NetworkRequest::headers() const {
    return headerList;
}
