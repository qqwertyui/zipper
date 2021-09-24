#ifndef ZIP_HPP
#define ZIP_HPP

#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Utils.hpp"
#include "Zip_structs.hpp"

class Zip {
public:
  // enums
  enum Job { LIST = 1, EXTRACT = 2 };

  typedef std::vector<CDFH*> vector_cdfh;
  typedef std::vector<LFH*> vector_lfh;

  // Structured zip data
  std::unique_ptr<ECDR> ecdr;
  std::vector<CDFH*> cdfhs;
  std::vector<LFH*> lfhs;

  // Raw zip data
  std::vector<unsigned char> raw;

  Zip(std::string filename);
  ~Zip();

  void list_files();
  unsigned int extract(const char *filename);
  unsigned int extract_all();

  bool is_good() const;
  Data *decompress(LFH *lfh);

private:
  static bool is_directory(CDFH *entry);

  // routines for reading zip format fields
  bool read_ecdr(std::vector<unsigned char> &data);
  bool read_cdfhs(std::vector<unsigned char> &data);
  bool read_lfhs(std::vector<unsigned char> &data);

  // Used internally by 'extract' method to find location of the file in
  // archive
  static LFH *find_file(const char *filename, std::vector<LFH *> &lfhs);

  CDFH *get_cdfh_from_lfh(LFH *lfh);
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