#pragma once
#include <cstdarg>
#include <string>

namespace zipcxx::utils::logging {
void info(const std::string &message);
void warn(const std::string &message);
void error(const std::string &message);
} // namespace zipcxx::utils::logging