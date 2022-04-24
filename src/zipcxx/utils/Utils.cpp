#include "Utils.hpp"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <zlib.h>

namespace zipcxx::utils {

DosTime::DosTime(uint16_t time, uint16_t date) {
  constexpr int SEC_BITMAP = 0x1f;
  constexpr int SEC_MULTIPLIER = 2;

  constexpr int MIN_BITMASK = 0x7ff;
  constexpr int MIN_SHIFT = 5;

  constexpr int HOUR_SHIFT = 11;

  constexpr int MDAY_BITMASK = 0x1f;

  constexpr int MON_BITMASK = 0x1ff;
  constexpr int MON_SHIFT = 5;
  constexpr int MON_SUBTRAHEND = 1;

  constexpr int YEAR_BASE = 1980;
  constexpr int YEAR_SHIFT = 9;
  constexpr int YEAR_SUBTRAHEND = 1900;

  timeinfo.tm_sec = (time & SEC_BITMAP) * SEC_MULTIPLIER;
  timeinfo.tm_min = (time & MIN_BITMASK) >> MIN_SHIFT;
  timeinfo.tm_hour = time >> HOUR_SHIFT;
  timeinfo.tm_mday = date & MDAY_BITMASK;
  timeinfo.tm_mon = ((date & MON_BITMASK) >> MON_SHIFT) - MON_SUBTRAHEND;
  timeinfo.tm_year = (YEAR_BASE + (date >> YEAR_SHIFT)) - YEAR_SUBTRAHEND;
}

time_t DosTime::getUnixTimestamp() { return mktime(&timeinfo); }

struct tm DosTime::getTmStruct() {
  return timeinfo;
}

void Chunk_manager::add(const std::vector<std::byte> &data) {
  this->entries.push_back(data);
  this->total_bytes += data.size();
}

std::vector<std::byte> Chunk_manager::to_bytearray() {
  std::vector<std::byte> bytes(this->total_bytes);
  unsigned int offset = 0;
  for (const auto &entry : entries) {
    memcpy(bytes.data() + offset, entry.data(), entry.size());
    offset += entry.size();
  }
  return bytes;
}

std::vector<std::byte> zlib_inflate(std::vector<std::byte> &input) {
  constexpr unsigned int CHUNKSZ = 0x1000;
  Chunk_manager container;
  z_stream infstream;

  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
  infstream.avail_in = static_cast<uInt>(input.size());
  infstream.next_in = reinterpret_cast<Bytef *>(input.data());

  int status = inflateInit2(&infstream, -MAX_WBITS);
  if (status != Z_OK) {
    throw std::runtime_error("Decompression error");
  }

  std::array<std::byte, CHUNKSZ> temp{};
  do {
    infstream.avail_out = CHUNKSZ;
    infstream.next_out = reinterpret_cast<Bytef *>(temp.data());

    status = inflate(&infstream, Z_NO_FLUSH);
    if (status != Z_OK && status != Z_STREAM_END) {
      inflateEnd(&infstream);
      throw std::runtime_error("Decompression error");
    }
    unsigned int bytesUsed = CHUNKSZ - infstream.avail_out;
    container.add(std::vector<std::byte>{temp.data(), temp.data() + bytesUsed});
  } while (status != Z_STREAM_END);

  inflateEnd(&infstream);
  return container.to_bytearray();
}

std::vector<std::byte> readFile(const std::string &filename,
                                std::ifstream::openmode flags) {
  std::ifstream file(filename, flags);
  if (not file.good()) {
    throw std::runtime_error("File reading error");
  }

  file.seekg(0, std::ios_base::end);
  unsigned int filesz = file.tellg();
  file.seekg(0, std::ios_base::beg);

  std::vector<std::byte> result(filesz);
  file.read(reinterpret_cast<char *>(result.data()), result.size());
  file.close();
  return result;
}

void writeFile(const std::string &filename, const std::vector<std::byte> &data,
               std::ofstream::openmode flags) {
  std::ofstream file(filename, flags);
  if (not file.good()) {
    throw std::runtime_error("File writing error");
  }
  file.write(reinterpret_cast<const char *>(data.data()), data.size());
  file.close();
}

} // namespace zipcxx::utils