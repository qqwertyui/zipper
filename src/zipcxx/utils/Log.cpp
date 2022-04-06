#include "Log.hpp"
#include <cstdio>

namespace zipcxx::utils::logging {

void info(const char *msg) { fprintf(stdout, "[+] %s", msg); }
void infof(const char *msg, ...) {
  va_list valist;
  va_start(valist, msg);

  fprintf(stdout, "[+] ");
  vfprintf(stdout, msg, valist);

  va_end(valist);
}

void error(const char *msg) { fprintf(stderr, "[!] %s", msg); }
void errorf(const char *msg, ...) {
  va_list valist;
  va_start(valist, msg);

  fprintf(stderr, "[!] ");
  vfprintf(stderr, msg, valist);

  va_end(valist);
}

} // namespace zipcxx::utils::logging