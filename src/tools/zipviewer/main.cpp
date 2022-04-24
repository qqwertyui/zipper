#include "../../zipcxx/Zip.hpp"
#include <iomanip>
#include <iostream>
#include <memory>
#include <utility>

template <typename T> void printValue(const std::string &name, const T &value) {
  std::cout << name << ": ";
  std::cout << std::setw(2) << std::setfill('0') << std::hex << value << "\n";
}

template <>
void printValue(const std::string &name, const std::vector<std::byte> &value) {
  std::cout << name << ": ";
  for (const auto &byte : value) {
    std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(byte)
              << " ";
  }
  std::cout << "\n";
}

void printHeaders(const zipcxx::ZipEntry &entry) {
  const auto &lfh{entry.getLfh()};
  std::cout << "--LFH--\n";
  printValue("ex_version", lfh.s.ex_version);
  printValue("gpbf", lfh.s.gpbf);
  printValue("c_method", lfh.s.c_method);
  printValue("mod_time", lfh.s.mod_time);
  printValue("mod_date", lfh.s.mod_date);
  printValue("crc32", lfh.s.crc32);
  printValue("c_size", lfh.s.c_size);
  printValue("uc_size", lfh.s.uc_size);
  printValue("name_length", lfh.s.name_length);
  printValue("extra_length", lfh.s.extra_length);
  printValue("name", lfh.name);
  if (lfh.extra.has_value()) {
    printValue("extra", lfh.extra.value());
  }

  const auto &cdfh{entry.getCdfh()};
  std::cout << "\n--CDFH--\n";
  printValue("version_made", cdfh.s.version_made);
  printValue("version_needed", cdfh.s.version_needed);
  printValue("gpbf", cdfh.s.gpbf);
  printValue("c_method", cdfh.s.c_method);
  printValue("mod_time", cdfh.s.mod_time);
  printValue("mod_date", cdfh.s.mod_date);
  printValue("crc32", cdfh.s.crc32);
  printValue("c_size", cdfh.s.c_size);
  printValue("uc_size", cdfh.s.uc_size);
  printValue("name_length", cdfh.s.name_length);
  printValue("extra_length", cdfh.s.extra_length);
  printValue("comment_length", cdfh.s.comment_length);
  printValue("disk_num", cdfh.s.disk_num);
  printValue("int_attributes", cdfh.s.int_attributes);
  printValue("ext_attributes", cdfh.s.ext_attributes);
  printValue("lh_offset", cdfh.s.lh_offset);
  printValue("name", cdfh.name);
  if (cdfh.extra.has_value()) {
    printValue("extra", cdfh.extra.value());
  }
  if (cdfh.comment.has_value()) {
    printValue("comment", cdfh.comment.value());
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " <archive> [<file1>,<file2>,...,<fileN>]\n";
    return 1;
  }
  try {
    auto archive = std::make_unique<zipcxx::Zip>(argv[1]);
    unsigned int files{static_cast<unsigned int>(argc - 2)};
    if (files != 0U) {
      for (unsigned int i = 0; i < files; i++) {
        printHeaders(archive->getEntryByFilename(argv[2 + i]));
      }
    } else {
      for (auto &entry : archive->getEntries()) {
        printHeaders(entry);
      }
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 2;
  }
  return 0;
}