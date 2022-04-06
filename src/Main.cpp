#include <gflags/gflags.h>

#include "Error.hpp"
#include "Gflags_defines.hpp"
#include "Log.hpp"
#include "Version.hpp"
#include "Zip.hpp"
#include <iostream>

using namespace zipper;

int main(int argc, char **argv) {
  gflags::SetUsageMessage("zipper.exe -f <input> -m [list]/extract\n");
  gflags::SetVersionString(Version::STRING);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_f.empty() == true) {
    fputs(gflags::ProgramUsage(), stderr);
    return Error::NOT_ENOUGH_ARGS;
  }

  Zip::Job operation{Zip::Job::LIST};
  if (FLAGS_m.compare("extract") == 0) {
    operation = Zip::Job::EXTRACT;
  }

  Zip *archive{nullptr};
  try {
    archive = new Zip(FLAGS_f);
  } catch (const std::runtime_error &e) {
    utils::logging::errorf("ZIP error: %s\n", e.what());
    return Error::INVALID_ZIP;
  }

  try {
    if (operation == Zip::Job::LIST) {
      for (auto &entry : archive->getEntries()) {
        std::cout << entry.getTimeAsString() << " | " << entry.getFilename()
                  << "\n";
      }
    } else if (operation == Zip::Job::EXTRACT) {
      archive->extractAll();
      utils::logging::info("Sucesfully extraced file(s)\n");
    }
  } catch (const std::exception &e) {
    utils::logging::errorf("Failed to process the file: %s\n", e.what());
    return Error::PROCESSING_ERROR;
  }

  return Error::OK;
}