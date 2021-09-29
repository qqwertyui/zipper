#include <gflags/gflags.h>

#include "Error.hpp"
#include "Log.hpp"
#include "Zip.hpp"
#include "Version.hpp"

DEFINE_string(f, "", "Input file");
DEFINE_string(m, "", "Mode (list, extract)");

int main(int argc, char **argv) {
  gflags::SetUsageMessage("zipper.exe -f <input> -m [list]/extract");
  gflags::SetVersionString(Version::STRING);
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

  Zip *archive = nullptr;
  try {
    archive = new Zip(FLAGS_f);
  } catch(const std::runtime_error &e) {
    Log::errorf("Invalid ZIP archive: %s\n", e.what());
    return Error::INVALID_ZIP;
  }

  try {
    if (operation == Zip::Job::LIST) {
      archive->list_files();
    } else if (operation == Zip::Job::EXTRACT) {
      archive->extract_all();
      Log::info("Sucesfully extraced file(s)");
    }
  } catch(const std::exception &e) {
    Log::errorf("Failed to process the file: %s\n", e.what());
    return Error::PROCESSING_ERROR;
  }

  return Error::OK;
}