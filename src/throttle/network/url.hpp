#pragma once

#include <string>

namespace throttle { namespace network {

class Url {
    class UrlImpl;
    const std::unique_ptr<UrlImpl> d;
public:
    Url();
    Url(const std::string &url);
    ~Url();

    bool isValid() const;
};

} }
