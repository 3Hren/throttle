#pragma once

#include <string>

namespace throttle {

namespace detail { namespace uri {
class UrlImpl;
} }

namespace network {

class Url {
    const std::unique_ptr<detail::uri::UrlImpl> d;
public:
    Url();
    Url(const std::string &url);
    ~Url();

    bool isValid() const;
};

} }
