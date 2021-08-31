#include "Utils.hpp"

#include <windows.h>
#include <zlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>

DosTime::DosTime(uint16_t time, uint16_t date) {
  this->second = (time & 0x1f) * 2;
  this->minute = (time & 0x7ff) >> 5;
  this->hour = time >> 11;

  this->day = date & 0x1f;
  this->month = (date & 0x1ff) >> 5;
  this->year = 1980 + (date >> 9);
}

Data::Data(unsigned char *data, unsigned int data_size) {
  this->data = data;
  this->data_size = data_size;
}

Data::Data() {}

Chunk_manager::Chunk_manager() {
  this->elements = 0;
  this->total_bytes = 0;
}

Chunk_manager::~Chunk_manager() {
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       i++) {
    delete[](*i)->data;
  }
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       i++) {
    delete (*i);
  }
}

void Chunk_manager::add(unsigned char *data, unsigned int data_size) {
  unsigned char *tmp = new unsigned char[data_size];
  memcpy(tmp, data, data_size);

  this->entry.push_back(new Data(tmp, data_size));
  this->elements += 1;
  this->total_bytes += data_size;
}

unsigned char *Chunk_manager::to_bytearray() {
  unsigned char *bytes = new unsigned char[this->total_bytes];
  unsigned int counter = 0;
  for (Data_vector::iterator i = this->entry.begin(); i != this->entry.end();
       i++) {
    memcpy(bytes + counter, (*i)->data, (*i)->data_size);
    counter += (*i)->data_size;
  }
  return bytes;
}

namespace Utils {
char *find_last_of(const char *text, const char *delimiter) {
  char *stripped_name = nullptr;
  char *tmp = strtok((char *)text, delimiter);
  do {
    stripped_name = tmp;
    tmp = strtok(nullptr, delimiter);
  } while (tmp != nullptr);

  return stripped_name;
}


Data *zlib_inflate(Data *input) {
  constexpr unsigned int CHUNKSZ = 0x1000;
  Chunk_manager container;
  z_stream infstream;

  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;
  infstream.avail_in = (uInt)input->data_size;
  infstream.next_in = input->data;

  int status = inflateInit2(&infstream, -MAX_WBITS);
  if (status != Z_OK) {
    return nullptr;
  }

  unsigned char temp[CHUNKSZ] = {0};
  int have = 0;
  do {
    infstream.avail_out = CHUNKSZ;
    infstream.next_out = temp;

    status = inflate(&infstream, Z_NO_FLUSH);
    if (status != Z_OK && status != Z_STREAM_END) {
      inflateEnd(&infstream);
      return nullptr;
    }
    have = CHUNKSZ - infstream.avail_out;
    container.add(temp, have);
  } while (status != Z_STREAM_END);

  inflateEnd(&infstream);
  Data *d = new Data(container.to_bytearray(), container.total_bytes);
  return d;
}


void create_directory(const char *filename) {
  char *buf = new char[strlen(filename) + 1];
  memset(buf, 0, strlen(filename + 1));
  memcpy(buf, filename, strlen(filename));

  char *tmp = buf;
  while ((tmp = strchr(tmp, '/')) != nullptr) {
    *tmp = '\0';
    CreateDirectory(buf, NULL);
    *tmp = '/';
    tmp++;
  }
  delete[] buf;
}

char *copy_string(const char *text) {
  size_t length = strlen(text);
  char *copy = new char[length + 1];
  memset(copy, 0, length + 1);
  memcpy(copy, text, length);

  return copy;
}

char *copy_string(const char *text, unsigned int length) {
  char *copy = new char[length + 1];
  memset(copy, 0, length + 1);
  memcpy(copy, text, length);

  return copy;
}
}