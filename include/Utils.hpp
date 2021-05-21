#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
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
  unsigned char *data;
  unsigned int data_size;

  Data(unsigned char *data, unsigned int data_size);
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

  void add(unsigned char *data, unsigned int datasz);
  unsigned char *to_bytearray();
};

namespace Utils {
/* Returns address of the last occurence of given character in string, alters
 * input string */
char *find_last_of(const char *text, const char *delimiter);

Data *zlib_inflate(Data *input);

/* Creates directory tree */
void create_directory(const char *filename);

/* Copies string, which needs to be deleted later */
char *copy_string(const char *text);

/* Copies length characters from text, almost the same as above */
char *copy_string(const char *text, unsigned int length);

}  // namespace Utils

#endif