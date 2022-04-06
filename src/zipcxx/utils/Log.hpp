#pragma once
#include <cstdarg>
#include <string>

namespace zipcxx::utils::logging {
void info(const char *msg);
void infof(const char *msg, ...);
void error(const char *msg);
void errorf(const char *msg, ...);
} // namespace zipcxx::utils::logging