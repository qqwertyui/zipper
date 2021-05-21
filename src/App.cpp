#include <getopt.h>

#include <cstdio>

#include "Error.hpp"
#include "Misc.hpp"
#include "Zip.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    Misc::help();
    return Error::NOT_ENOUGH_ARGS;
  }

  const char *target = nullptr;
  unsigned int operation = Zip::Job::LIST;
  int flag = Zip::Flag::NONE;

  // argc and argv are modified below to make it possible to put the first
  // argument without dash
  char c;
  while ((c = getopt((argc - 1), (argv + 1), "vhx:")) != -1) {
    switch (c) {
      case 'x': {
        operation = Zip::Job::EXTRACT;
        target = optarg;
        break;
      }
      case 'v': {
        Misc::version();
        return Error::OK;
      }
      case 'h': {
        Misc::help();
        return Error::OK;
      }
      default: {
        return Error::WRONG_ARGUMENT;
      }
    }
  }

  Zip z1(argv[1]);
  if (z1.is_good() == false) {
    puts("File is not valid ZIP archive");
    return Error::INVALID_ZIP;
  }

  if (operation == Zip::Job::LIST) {
    z1.list_files((Zip::Flag)flag);
  } else if (operation == Zip::Job::EXTRACT) {
    unsigned int status = Misc::is_wildcard_present(target)
                              ? z1.extract_all((Zip::Flag)flag)
                              : z1.extract(target, (Zip::Flag)flag);
    if (status == Status::OK) {
      puts("Sucesfully extraced file(s)");
    } else if (status == Status::FILE_NOT_FOUND) {
      puts("Requested file doesn't exist");
    } else if (status == Status::DECOMPRESS_ERROR) {
      puts("Failed to decompress file");
    } else if (status == Status::IO_ERROR) {
      puts("Input/Output error");
    } else if (status == Status::WRONG_NAME) {
      puts("Given file path is wrong");
    } else {
      puts("Some unknown error ocurred");
    }
  }
  puts("");
  return Error::OK;
}