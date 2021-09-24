#include "Log.hpp"
#include <cstdio>

void Log::info(const char *msg) { fprintf(stdout, "[+] %s", msg); }
void Log::infof(const char *msg, ...) { 
    va_list valist;
    va_start(valist, msg);

    fprintf(stdout, "[+] ");
    vfprintf(stdout, msg, valist);

    va_end(valist);
}


void Log::error(const char *msg) { fprintf(stderr, "[!] %s", msg); }
void Log::errorf(const char *msg, ...) { 
    va_list valist;
    va_start(valist, msg);

    fprintf(stderr, "[!] ");
    vfprintf(stderr, msg, valist);

    va_end(valist);
}