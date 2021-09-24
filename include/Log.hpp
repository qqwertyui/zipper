#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <cstdarg>

namespace Log {
    void info(const char *msg);
    void infof(const char *msg, ...);
    void error(const char *msg);
    void errorf(const char *msg, ...);
}

#endif