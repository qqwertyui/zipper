#pragma once

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace zipcxx::utils {
class DosTime {
public:
  DosTime(uint16_t time, uint16_t date);

  time_t getUnixTimestamp();
  struct tm getTmStruct();

private:
  struct tm timeinfo;
};

class Data {
public:
  std::byte *data;
  unsigned int data_size;

  Data(std::byte *data, unsigned int data_size);
  Data(std::vector<std::byte> &data);
  Data() = default;
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

/* Returns address of the last occurence of given character in string, alters
 * input string */
char *find_last_of(const char *text, const char *delimiter);

std::vector<std::byte> zlib_inflate(Data *input);

std::vector<std::byte>
readFile(const std::string &filename,
         std::ifstream::openmode flags = std::ifstream::in);

void writeFile(const std::string &filename, const std::vector<std::byte> &data,
               std::ofstream::openmode flags = std::ofstream::out);
} // namespace zipcxx::utils