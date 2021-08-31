#include "Log.hpp"
#include <cstdio>

void Log::info(std::string msg) {
    fprintf(stdout, "[+] %s", msg.c_str());
}

void Log::error(std::string msg) {
    fprintf(stderr, "[!] %s", msg.c_str());
}