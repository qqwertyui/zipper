#include "Utils.hpp"

#include <zlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace zipper::utils {
DosTime::DosTime(uint16_t time, uint16_t date) {
  timeinfo.tm_sec = (time & 0x1f) * 2;
  timeinfo.tm_min = (time & 0x7ff) >> 5;
  timeinfo.tm_hour = time >> 11;
  timeinfo.tm_mday = date & 0x1f;
  timeinfo.tm_mon = ((date & 0x1ff) >> 5) - 1;
  timeinfo.tm_year = (1980 + (date >> 9)) - 1900;
}

time_t DosTime::getUnixTimestamp() { return mktime(&timeinfo); }

struct tm DosTime::getTmStruct() {
  return timeinfo;
}

Data::Data(std::byte *data, unsigned int data_size) {
  this->data = data;
  this->data_size = data_size;
}

Data::Data(std::vector<std::byte> &data) {
  this->data = data.data();
  this->data_size = data.size();
}

Chunk_manager::Chunk_manager() {
  this->elements = 0;
  this->total_bytes = 0;
}

Chunk_manager::~Chunk_manager() {
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       ++i) {
    delete[](*i)->data;
  }
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       ++i) {
    delete (*i);
  }
}

void Chunk_manager::add(std::byte *data, unsigned int data_size) {
  std::byte *tmp = new std::byte[data_size];
  memcpy(tmp, data, data_size);

  this->entry.push_back(new Data(tmp, data_size));
  this->elements += 1;
  this->total_bytes += data_size;
}

std::byte *Chunk_manager::to_bytearray() {
  std::byte *bytes = new std::byte[this->total_bytes];
  unsigned int counter = 0;
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       ++i) {
    memcpy(bytes + counter, (*i)->data, (*i)->data_size);
    counter += (*i)->data_size;
  }
  return bytes;
}

char *find_last_of(const char *text, const char *delimiter) {
  char *stripped_name = nullptr;
  char *tmp = strtok((char *)text, delimiter);
  do {
    stripped_name = tmp;
    tmp = strtok(nullptr, delimiter);
  } while (tmp != nullptr);

  return stripped_name;
}

std::vector<std::byte> zlib_inflate(Data *input) {
  constexpr unsigned int CHUNKSZ = 0x1000;
  Chunk_manager container;
  z_stream infstream;

  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
  infstream.avail_in = (uInt)input->data_size;
  infstream.next_in = (Bytef *)input->data;

  int status = inflateInit2(&infstream, -MAX_WBITS);
  if (status != Z_OK) {
    throw std::runtime_error("Decompression error");
  }

  std::byte temp[CHUNKSZ] = {};
  do {
    infstream.avail_out = CHUNKSZ;
    infstream.next_out = (Bytef *)temp;

    status = inflate(&infstream, Z_NO_FLUSH);
    if (status != Z_OK && status != Z_STREAM_END) {
      inflateEnd(&infstream);
      throw std::runtime_error("Decompression error");
    }
    int have = CHUNKSZ - infstream.avail_out;
    container.add(temp, have);
  } while (status != Z_STREAM_END);

  inflateEnd(&infstream);
  std::vector<std::byte> result(container.total_bytes);
  memcpy(result.data(), container.to_bytearray(), container.total_bytes);
  return result;
}

std::vector<std::byte> readFile(const std::string &filename,
                                std::ifstream::openmode flags) {
  std::ifstream file(filename, flags);
  if (file.good() == false) {
    throw std::runtime_error("File reading error");
  }

  file.seekg(0, std::ios_base::end);
  unsigned int filesz = file.tellg();
  file.seekg(0, std::ios_base::beg);

  std::vector<std::byte> result(filesz);
  file.read((char *)result.data(), result.capacity());
  file.close();
  return result;
}

void writeFile(const std::string &filename, const std::vector<std::byte> &data,
               std::ofstream::openmode flags) {
  std::ofstream file(filename, flags);
  if (file.good() == false) {
    throw std::runtime_error("File writing error");
  }
  file.write((char *)data.data(), data.size());
  file.close();
}

} // namespace zipper::utils