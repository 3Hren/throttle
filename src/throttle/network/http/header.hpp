#pragma once

#include <string>

namespace throttle { namespace network { namespace http {

struct Header {
    std::string name;
    std::string value;
};

} } }
