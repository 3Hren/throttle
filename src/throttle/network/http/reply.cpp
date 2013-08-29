#include "reply.hpp"

using namespace throttle::network::http;

NetworkReply::NetworkReply(NetworkReply &&other)  :
    headerList(std::move(other.headerList)),
    body(std::move(other.body))
{}

HeaderList &NetworkReply::headers() {
    return headerList;
}

const HeaderList &NetworkReply::headers() const {
    return headerList;
}
