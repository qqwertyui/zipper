#include <gflags/gflags.h>

#include "Error.hpp"
#include "Zip.hpp"
#include "Log.hpp"


namespace Version {
constexpr int MAJOR = 0;
constexpr int MINIOR = 1;
char prepared_string[4];
} // namespace Version

DEFINE_string(f, "", "Input file");
DEFINE_string(m, "", "Mode");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("zipper.exe -f <input> -m [list]/extract\n");

  sprintf(Version::prepared_string, "%d.%d", Version::MAJOR, Version::MINIOR);
  gflags::SetVersionString(Version::prepared_string);

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_f.empty() == true) {
    fputs(gflags::ProgramUsage(), stderr);
    return Error::NOT_ENOUGH_ARGS;
  }

  Zip::Job operation = Zip::Job::LIST;
  if (FLAGS_m.compare("extract") == 0) {
    operation = Zip::Job::EXTRACT;
  } else {
    operation = Zip::Job::LIST;
  }

  Zip archive(FLAGS_f);
  if (archive.is_good() == false) {
    Log::error("Invalid ZIP archive");
    return Error::INVALID_ZIP;
  }

  if (operation == Zip::Job::LIST) {
    archive.list_files();
  } else if (operation == Zip::Job::EXTRACT) {
    unsigned int status = archive.extract_all();
    if(status != Status::OK) {
        if (status == Status::FILE_NOT_FOUND) {
        Log::error("File doesn't exist");
        } else if (status == Status::DECOMPRESS_ERROR) {
        Log::error("Decompression failure");
        } else if (status == Status::IO_ERROR) {
        Log::error("Input/Output error");
        } else if (status == Status::WRONG_NAME) {
        Log::error("Wrong file path");
        } else {
        Log::error("Unkown error");
        }
        return status;
    }
    Log::info("Sucesfully extraced file(s)");
  }
  
  return Error::OK;
}