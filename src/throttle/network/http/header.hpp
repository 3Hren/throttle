#pragma once

#include <string>
#include <list>

#include <boost/lexical_cast.hpp>

#define DECLARE_KNOWN_HEADER(__class__, __value_type__) \
    struct __class__ { \
    static const char name[]; \
    typedef __value_type__ value_type; \
    };

namespace throttle { namespace network { namespace http {

struct Header {
    DECLARE_KNOWN_HEADER(ContentType, std::string)
    DECLARE_KNOWN_HEADER(ContentLength, int)
    DECLARE_KNOWN_HEADER(Location, std::string)

    std::string name;
    std::string value;

    Header(const std::string &name, const std::string &value) :
        name(name),
        value(value)
    {}
};

namespace Match {

struct HeaderName {
    const std::string &name;
    bool operator()(const Header &header) const {
        return header.name == name;
    }
};

}

class HeaderList {
public:
    struct Container {
        typedef typename std::list<Header> Type;
    };

private:
    Container::Type headers;

public:
    Container::Type all() const;

    uint16_t size() const;

    bool has(const std::string &name) const;
    template<typename T>
    bool has() const {
        return has(T::name);
    }

    std::string get(const std::string &name) const;
    template<typename T>
    typename T::value_type get() const {
        return boost::lexical_cast<typename T::value_type>(get(T::name));
    }

    void set(const std::string &name, const std::string &value);
    void set(const Header &header);
    void set(Header &&header);
    template<typename T>
    void set(typename T::value_type value) {
        set(T::name, boost::lexical_cast<std::string>(value));
    }

    void reset(const Container::Type &headers);
    void reset(Container::Type &&headers);

private:
    Container::Type::const_iterator findByName(const std::string &name) const;
    Container::Type::iterator findByName(const std::string &name);
};

} } }
