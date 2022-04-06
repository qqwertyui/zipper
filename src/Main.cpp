#include <gflags/gflags.h>

#include "GflagsDefines.hpp"
#include "Version.hpp"
#include "zipcxx/Zip.hpp"
#include <iostream>

enum Status { ok, not_enough_args, invalid_zip, processing_error };

int main(int argc, char **argv) {
  gflags::SetUsageMessage("zipper.exe -f <input> -m [list,extract]\n");
  gflags::SetVersionString(Version::STRING);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_f.empty() == true) {
    std::cerr << gflags::ProgramUsage() << "\n";
    return Status::not_enough_args;
  }

  zipcxx::Zip *archive{nullptr};
  try {
    archive = new zipcxx::Zip(FLAGS_f);
  } catch (const std::runtime_error &e) {
    std::cerr << "ZIP error: " << e.what() << "\n";
    return Status::invalid_zip;
  }

  try {
    if (FLAGS_m.compare("list") == 0) {
      for (auto &entry : archive->getEntries()) {
        std::cout << entry.getTimeAsString() << " | " << entry.getFilename()
                  << "\n";
      }
    } else if (FLAGS_m.compare("extract") == 0) {
      archive->extractAll();
      std::cout << "Sucesfully extraced file(s)\n";
    } else {
      std::cout << "Unknown operation, possible are: [list,extract]\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Failed to process the file: " << e.what() << "\n";
    return Status::processing_error;
  }

  return Status::ok;
}