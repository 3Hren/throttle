#pragma once

#include <string>

namespace throttle { namespace network { namespace http {

struct Header {
    std::string name;
    std::string value;

    Header(const std::string &name, const std::string &value) :
        name(name),
        value(value)
    {}
};

} } }
