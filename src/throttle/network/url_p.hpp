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

enum Code {
    NoErrors = 0,
    NoProtocolFound
};

struct state_t {
    size_t pos;
    protocol::type::Type current_protocol;
    Code error_code;
};

state_t parse_protocol(const char *url, size_t size) {
    state_t state { 0, protocol::type::Unknown, NoProtocolFound };
    LOG_DEBUG("%d vs %d", size, MIN_PROTOCOL_SIZE);
    if (size < MIN_PROTOCOL_SIZE)
        return state;

    std::list<ProtocolPair> protocols = {
        ProtocolPair::make<protocol::Http>(),
        ProtocolPair::make<protocol::Ftp>()
    };

    for (; state.pos < size; ++state.pos) {
        protocols.remove_if([&url, &state, &protocols](const ProtocolPair &pair){
            return url[state.pos] != pair.value[state.pos];
        });
        LOG_DEBUG("%d, %d", state.pos, protocols.size());

        if (protocols.empty()) {
            LOG_DEBUG("no more protocols left, returning %d", state.pos);
            return state;
        }

        if (protocols.size() == 1) {
            const ProtocolPair &pair = protocols.front();
            const int len = std::strlen(pair.value);

            if (std::strncmp(url + state.pos, pair.value + state.pos, len) == 0) {
                state.pos = len;
                state.current_protocol = pair.type;
                state.error_code = NoErrors;
            }
            return state;
        }
    }
    return state;
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
        LOG_DEBUG("%d. error_code? %s", state.pos, state.error_code);
        if (state.error_code != protocol::detail::parse::NoErrors || state.pos == url.size())
            return;

        if (state.current_protocol == protocol::type::Http) {
            state.pos += parseHostPort(url.data() + state.pos);
        }
    }

    int parseHostPort(const char *url) {
        LOG_DEBUG("preparing to parse \"%s\" for hostport", url);
        return 0;
    }
};

} }
