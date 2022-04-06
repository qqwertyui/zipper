#pragma once
#include <cstdarg>
#include <string>

namespace zipper::utils::logging {
void info(const char *msg);
void infof(const char *msg, ...);
void error(const char *msg);
void errorf(const char *msg, ...);
} // namespace zipper::utils::logging