#ifndef MISC_HPP
#define MISC_HPP

namespace Misc {
constexpr int VERSION = 0;
constexpr int SUBVERSION = 1;

constexpr const char *WILDCARD = "-a";

void help();
void version();
bool is_wildcard_present(const char *text);
}  // namespace Misc

#endif