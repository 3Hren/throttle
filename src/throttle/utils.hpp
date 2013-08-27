#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>

#include "config.hpp"

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

}
