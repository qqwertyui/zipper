#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class DosTime {
public:
  DosTime(uint16_t time, uint16_t date);

  uint8_t second;
  uint8_t minute;
  uint8_t hour;

  uint8_t day;
  uint8_t month;
  uint16_t year;
};

class Data {
public:
  std::byte *data;
  unsigned int data_size;

  Data(std::byte *data, unsigned int data_size);
  Data();
};

typedef std::vector<Data *> Data_vector;

class Chunk_manager {
public:
  unsigned int elements;
  unsigned int total_bytes;
  Data_vector entry;

  Chunk_manager();
  ~Chunk_manager();

  void add(std::byte *data, unsigned int datasz);
  std::byte *to_bytearray();
};

namespace Utils {
/* Returns address of the last occurence of given character in string, alters
 * input string */
char *find_last_of(const char *text, const char *delimiter);

std::vector<std::byte> zlib_inflate(Data *input);

std::vector<std::byte>
read_file(std::string &filename,
          std::ifstream::openmode flags = std::ifstream::in);

void write_file(std::string &filename, std::vector<std::byte> &data,
                std::ofstream::openmode flags = std::ofstream::out);
} // namespace Utils