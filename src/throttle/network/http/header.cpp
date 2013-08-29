#include "header.hpp"

using namespace throttle::network::http;

const char Header::ContentType::name[] = "Content-Type";
const char Header::ContentLength::name[] = "Content-Length";
const char Header::Location::name[] = "Location";

HeaderList::Container::Type HeaderList::all() const {
    return headers;
}

uint16_t HeaderList::size() const {
    return headers.size();
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
    auto it = findByName(header.name);
    if (it != headers.end())
        *it = header;
    else
        headers.push_back(header);
}

void HeaderList::set(Header &&header) {
    auto it = findByName(header.name);
    if (it != headers.end())
        *it = std::move(header);
    else
        headers.emplace_back(std::move(header));
}

void HeaderList::reset(const Container::Type &headers) {
    this->headers = headers;
}

void HeaderList::reset(Container::Type &&headers) {
    this->headers = std::move(headers);
}

HeaderList::Container::Type::const_iterator HeaderList::findByName(const std::string &name) const {
    Match::HeaderName match{ name };
    return std::find_if(headers.begin(), headers.end(), match);
}

HeaderList::Container::Type::iterator HeaderList::findByName(const std::string &name) {
    Match::HeaderName match{ name };
    return std::find_if(headers.begin(), headers.end(), match);
}
