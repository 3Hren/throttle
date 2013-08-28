#include "reply.hpp"

using namespace throttle::network::http;

namespace Match {

struct HeaderName {
    const std::string &name;
    bool operator()(const Header &header) const {
        return header.name == name;
    }
};

}

NetworkReply::NetworkReply(NetworkReply &&other)  :
    headers(std::move(other.headers)),
    body(std::move(other.body))
{}

bool NetworkReply::hasHeader(const std::string &name) const {
    Match::HeaderName match{ name };
    return std::find_if(headers.begin(), headers.end(), match) != headers.end();
}

std::string NetworkReply::getHeader(const std::string &name) const {
    Match::HeaderName match{ name };
    auto it = std::find_if(headers.begin(), headers.end(), match);
    if (it != headers.end())
        return it->value;
    return std::string();
}

void NetworkReply::setHeader(const Header &header) {
    headers.emplace_back(header);
}

void NetworkReply::setHeader(Header &&header) {
    headers.emplace_back(std::forward<Header>(header));
}
