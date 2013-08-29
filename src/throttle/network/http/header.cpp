#include "header.hpp"

using namespace throttle::network::http;

const char Header::ContentType::name[] = "Content-Type";
const char Header::ContentLength::name[] = "Content-Length";
const char Header::Location::name[] = "Location";


std::list<Header> HeaderList::all() const {
    return headers;
}

bool HeaderList::has(const std::string &name) const {
    Match::HeaderName match{ name };
    return std::find_if(headers.begin(), headers.end(), match) != headers.end();
}

std::string HeaderList::get(const std::string &name) const {
    Match::HeaderName match{ name };
    auto it = std::find_if(headers.begin(), headers.end(), match);
    if (it != headers.end())
        return it->value;
    return std::string();
}

void HeaderList::set(const std::string &name, const std::string &value) {
    set({name, value});
}

void HeaderList::set(const Header &header) {
    headers.push_back(header);
}

void HeaderList::set(Header &&header) {
    headers.emplace_back(std::move(header));
}
