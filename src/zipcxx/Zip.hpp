#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "ZipEntry.hpp"
#include "ZipStructs.hpp"
#include "utils/Utils.hpp"

namespace zipcxx {
class Zip {
public:
  Zip(const std::string &filename);

  std::vector<std::string> getFilenames() const;
  void extract(const std::string &filename);
  void extractAll();

  std::vector<ZipEntry> &getEntries();
  ZipEntry &getEntryByFilename(const std::string &filename);

private:
  void checkIfValidArchive(const std::vector<std::byte> &rawBytes);
  static bool isValidLFHSignature(const std::vector<std::byte> &inputSignature);
  static bool isDirectory(const CDFH &entry);

  std::unique_ptr<ECDR> readEcdr(const std::vector<std::byte> &data);
  std::vector<CDFH> readCdfhs(const std::vector<std::byte> &data);
  std::vector<LFH> readLfhs(const std::vector<std::byte> &data,
                            const std::vector<CDFH> &cdfhs);

  std::vector<std::byte> decompress(LFH &lfh);

  std::unique_ptr<ECDR> ecdr;
  std::vector<ZipEntry> entries;
};

enum class Compression {
  STORED = 0,
  SHRUNK = 1,
  COMPRESSION_FACT_1 = 2,
  COMPRESSION_FACT_2 = 3,
  COMPRESSION_FACT_3 = 4,
  COMPRESSION_FACT_4 = 5,
  IMPLODED = 6,
  RESERVED_1 = 7,
  DEFLATE = 8,
  DEFLATE_64 = 9,
  PKWARE_COMP = 10,
  RESERVED_2 = 11,
  BZIP_2 = 12,
  RESERVED_3 = 13,
  LZMA = 14,
  RESERVED_4 = 15,
  CMPSC = 16,
  RESERVED_5 = 17,
  IBM_TERSE = 18,
  LZ77 = 19,
  DEPRECATED = 20,
  ZSTANDARD = 93,
  MP3 = 94,
  XZ = 95,
  JPEG = 96,
  WAVPACK = 97,
  PPMD_V1 = 98,
  AEX = 99
};
} // namespace zipcxx