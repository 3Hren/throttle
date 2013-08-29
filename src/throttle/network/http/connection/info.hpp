#pragma once

#include <sstream>

#include <curl/curl.h>

#include "utils.hpp"
#include "network/http/request.hpp"
#include "network/http/reply.hpp"
#include "network/http/callbacks.hpp"

namespace throttle { namespace network { namespace http { namespace connection {

struct Info {
    CURL *easy;

    const NetworkRequest request;
    const Callbacks callbacks;

    std::string data;

    std::stringstream bodyStream;
    NetworkReply reply;

    Info(const NetworkRequest &request, const Callbacks &callbacks);
    ~Info();

    void send(CURLM *multi) const;

private:
    static size_t writeCallback(char *data, size_t size, size_t nmemb, connection::Info *info);
    static size_t headerCallback(char *data, size_t size, size_t nmemb, connection::Info *info);
    struct curl_slist *packHeaders(const std::list<Header> &headers) const;
    void addHeader(const std::string &name, const std::string &value, struct curl_slist **headers) const;
};

} } } }
