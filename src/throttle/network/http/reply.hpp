#pragma once

#include <string>
#include <list>

#include "header.hpp"

namespace throttle { namespace network { namespace http {

struct HttpReply {
    std::list<Header> headers;
    std::string body;

    HttpReply() = default;
    HttpReply(HttpReply &&other) :
        headers(std::move(other.headers)),
        body(std::move(other.body))
    {}

    bool hasHeader(const std::string &name) const {
        return std::find_if(headers.begin(), headers.end(), [&name](const Header &header){
            return header.name == name;
        }) != headers.end();
    }

    std::string getHeader(const std::string &name) const {
        return std::find_if(headers.begin(), headers.end(), [&name](const Header &header){
            return header.name == name;
        })->value;
    }

    void setHeader(const Header &header) {
        headers.emplace_back(header);
    }
};

} } }
