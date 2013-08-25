#include "network_request.hpp"

std::string NetworkRequest::getUrl() const {
    return url;
}

void NetworkRequest::setUrl(const std::string &url)
{
    this->url = url;
}
