#include "Log.hpp"
#include <fmt/core.h>

namespace zipcxx::utils::logging {

void info(const std::string &message) { fmt::print("[+] {}\n", message); }

void warn(const std::string &message) { fmt::print("[!] {}\n", message); }

void error(const std::string &message) { fmt::print("[-] {}\n", message); }

} // namespace zipcxx::utils::logging