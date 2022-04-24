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

class Chunk_manager {
public:
  unsigned int total_bytes{};
  std::vector<std::vector<std::byte>> entries;

  void add(const std::vector<std::byte> &data);
  std::vector<std::byte> to_bytearray();
};

std::vector<std::byte> zlib_inflate(std::vector<std::byte> &input);

std::vector<std::byte>
readFile(const std::string &filename,
         std::ifstream::openmode flags = std::ifstream::in);

void writeFile(const std::string &filename, const std::vector<std::byte> &data,
               std::ofstream::openmode flags = std::ofstream::out);
} // namespace zipcxx::utils