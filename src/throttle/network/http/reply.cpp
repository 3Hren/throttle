#include "reply.hpp"

using namespace throttle::network::http;

NetworkReply::NetworkReply(NetworkReply &&other)  :
    headerList(std::move(other.headerList)),
    body(std::move(other.body))
{}

void NetworkReply::setRequest(NetworkRequest &&request) {
    this->request = std::move(request);
}

HeaderList &NetworkReply::headers() {
    return headerList;
}

const HeaderList &NetworkReply::headers() const {
    return headerList;
}

std::string NetworkReply::getBody() const {
    return body;
}

void NetworkReply::setBody(const std::string &body) {
   this->body = body;
}
