#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>

#include "config.hpp"

#define DECLARE_NONCOPYABLE(__class__) \
    __class__(const __class__ &other) = delete; \
    __class__& operator =(const __class__ &other) = delete;

#define UNUSED(T) \
    (void)T;

inline void LogMessage(boost::format &message) {
    std::cout << message.str() << std::endl;
}

template<typename T, typename... Args>
void LogMessage(boost::format &message, T&& arg, Args&&... args) {
    message % arg;
    LogMessage(message, std::forward<Args>(args)...);
}

template<typename... Args>
void LogMessage(const std::string &format, Args&&... args) {
    boost::format message(format);
    LogMessage(message, std::forward<Args>(args)...);
}

#ifdef THROTTLE_DEBUG
#define LOG_DEBUG(__format, __args...) \
    do { \
        boost::format __message("%s(%d) -- %s"); \
        __message % __func__ % __LINE__ % __format; \
        LogMessage(__message.str(), __args); \
    } while (0);
#else
#define LOG_DEBUG(__format, __args...)
#endif

namespace helpers {

inline std::string trimmed(const char *begin, const char *end) {
    while (begin < end && isspace(*begin))
        ++begin;
    while (begin < end && isspace(*(end - 1)))
        --end;

    return std::string(begin, end);
}

template<typename T, size_t N>
constexpr int static_strlen(const T (&)[N]) {
    return N - 1;
}

template<typename... Rest>
struct Min;

template<typename M>
struct Min<M> {
    enum { value = static_strlen(M::value) };
};

template<typename M, typename First>
struct Min<M, First> {
    enum {
        value = static_strlen(First::value) < static_strlen(M::value) ?
        Min<First>::value :
        Min<M>::value
    };
};

template<typename M, typename First, typename... Rest>
struct Min<M, First, Rest...> {
    enum {
        value = static_strlen(First::value) < static_strlen(M::value) ?
        Min<First, Rest...>::value :
        Min<M, Rest...>::value
    };
};

}
