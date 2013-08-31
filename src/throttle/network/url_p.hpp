#pragma once

#include <iostream>
#include <list>
#include <cstring>

#include "utils.hpp"
#include "url.hpp"

namespace throttle { namespace network {

namespace protocol {
namespace type {

enum Type {
    Unknown,
    Http,
    Ftp
};

} // namespace type

#define DECLARE(__class, __type, __name) \
    struct __class { \
        static const type::Type type; \
        static const char value[];\
    }; \
    const type::Type __class::type = __type; \
    const char __class::value[] = __name

DECLARE(Http,   type::Http,     "http://");
DECLARE(Ftp,    type::Ftp,      "ftp://");
//DECLARE(Afs,    type::Afs,      "afs://");
//DECLARE(News,   type::News,     "news:");
//DECLARE(Nttp,   "nttp:");
//DECLARE(Mid,    "mid:");
//DECLARE(Cid,    "cid:");
//DECLARE(Mailto, "mailto:");
//DECLARE(Wais,   "wais://");

constexpr int MIN_PROTOCOL_SIZE = helpers::Min<
        protocol::Http,
        protocol::Ftp
//        protocol::Afs,
//        protocol::News,
//        protocol::Nttp,
//        protocol::Mid,
//        protocol::Cid,
//        protocol::Mailto,
//        protocol::Wais
    >::value;

struct ProtocolPair {
    protocol::type::Type type;
    const char* value;

    template<typename P>
    static ProtocolPair make() {
        return { P::type, P::value };
    }
};


namespace detail { namespace parse {

struct state_t {
    const size_t pos;
    const protocol::type::Type current_protocol;

    state_t(size_t pos = 0, protocol::type::Type current_protocol = protocol::type::Unknown) :
        pos(pos),
        current_protocol(current_protocol)
    {}
};

const std::list<ProtocolPair> PROTOCOLS_PAIR_LIST = {
    ProtocolPair::make<protocol::Http>(),
    ProtocolPair::make<protocol::Ftp>()
};

struct match_char_t {
    const char *url;
    size_t pos;

    bool operator ()(const ProtocolPair &pair) const {
        return url[pos] != pair.value[pos];
    }
};

state_t compare_remaining(const char *url, size_t pos, ProtocolPair &&pair) {
    const int len = std::strlen(pair.value);
    if (std::strncmp(url + pos, pair.value + pos, len) == 0)
        return state_t(len, pair.type);

    return state_t();
}

state_t parse_protocol(const char *url, size_t size) {
    LOG_DEBUG("%d vs %d", size, MIN_PROTOCOL_SIZE);
    if (size < MIN_PROTOCOL_SIZE)
        return state_t();

    std::list<ProtocolPair> protocols = PROTOCOLS_PAIR_LIST;
    size_t pos = 0;
    while (pos < size) {
        protocols.remove_if(match_char_t{ url, pos });
        LOG_DEBUG("%d, %d", pos, protocols.size());

        if (protocols.empty()) {
            LOG_DEBUG("no more protocols left, returning %d", pos);
            return state_t();
        }

        if (protocols.size() == 1)
            return compare_remaining(url, pos, std::move(protocols.front()));
        ++pos;
    }
    return state_t();
}

state_t parse_protocol(const char *url) {
    return parse_protocol(url, std::strlen(url));
}

} } } // namespace protocol::detail::parse

class Url::UrlImpl {

public:
    bool valid;
    protocol::type::Type current_protocol;

    UrlImpl() :
        valid(false)
    {}

    ~UrlImpl() {
    }

    void parse(const std::string& url) {
        parseAddress(url);
    }

    void parseAddress(const std::string &url) {
        protocol::detail::parse::state_t state = protocol::detail::parse::parse_protocol(url.data(), url.size());
        LOG_DEBUG("%d. protocol: %s", state.current_protocol);
        if (state.current_protocol == protocol::type::Unknown || state.pos == url.size())
            return;

        if (state.current_protocol == protocol::type::Http) {
        }
    }

    int parseHostPort(const char *url) {
        LOG_DEBUG("preparing to parse \"%s\" for hostport", url);
        return 0;
    }
};

} }
