#pragma once

#include <string>

class NetworkRequest {
    std::string url;

public:
    std::string getUrl() const;
    void setUrl(const std::string &url);
};
