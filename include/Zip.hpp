#ifndef ZIP_HPP
#define ZIP_HPP

#include <cstddef>
#include <vector>
#include <memory>
#include <tuple>
#include <stdexcept>

#include "Utils.hpp"
#include "Zip_structs.hpp"

class ZipEntry;

class Zip {
public:
  // enums
  enum Job { 
      LIST = 1, 
      EXTRACT = 2 
  };

  Zip(std::string filename);
  ~Zip();

  void list_files() const;
  void extract(std::string &filename);
  void extract_all();

  std::vector<ZipEntry*> get_entries() const;

private:
  static bool is_directory(CDFH *entry);
  
  std::unique_ptr<ECDR> read_ecdr(std::vector<std::byte> &data);
  std::vector<CDFH*> read_cdfhs(std::vector<std::byte> &data);
  std::vector<LFH*> read_lfhs(std::vector<std::byte> &data, std::vector<CDFH*> &cdfhs);

  // Structured zip data
  std::unique_ptr<ECDR> ecdr;
  std::vector<ZipEntry*> entries;

  std::vector<std::byte> decompress(LFH *lfh);
  ZipEntry* get_entry_by_filename(std::string &filename);
};

class ZipEntry {
public:
    ZipEntry(const CDFH &cdfh, const LFH &lfh);
    ~ZipEntry();

    const std::string& get_name() const;
    const DosTime& get_time() const;
    CDFH& get_cdfh();
    LFH& get_lfh();

private:
    std::string filename;
    DosTime *time;

    CDFH cdfh;
    LFH lfh;
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

#endif