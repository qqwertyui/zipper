#include "ZipStructs.hpp"

#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace zipcxx {

Hexdumpable::Hexdumpable(const std::vector<std::byte> &raw_bytes)
    : raw_bytes(raw_bytes) {}

Hexdumpable::Hexdumpable(const Hexdumpable &old) : raw_bytes(old.raw_bytes) {}

Hexdumpable::Hexdumpable(Hexdumpable &&old)
    : raw_bytes(std::move(old.raw_bytes)) {}

void Hexdumpable::hexdump() {
  int i = 0;
  for (const auto &byte : raw_bytes) {
    if (i % 0x10 == 0) {
      puts("");
    }
    printf("%.2x ", static_cast<uint8_t>(byte));
    i++;
  }
}

ECDR::ECDR() : Hexdumpable() {}

ECDR::ECDR(std::vector<std::byte> &data) : Hexdumpable(data) {
  memcpy(&this->s, data.data(), sizeof(ECDR_static));

  if (this->s.comment_length > 0) {
    char *commentBegin{reinterpret_cast<char *>(data.data()) +
                       sizeof(ECDR_static)};
    this->comment = std::string(commentBegin, this->s.comment_length);
  }
}

LFH::LFH() : Hexdumpable() {}

LFH::LFH(std::vector<std::byte> &data) : Hexdumpable(data) {
  if (data.size() < sizeof(LFH_static)) {
    throw std::runtime_error("Corrupted archive (invalid LFH entry length)");
  }
  memcpy(&this->s, data.data(), sizeof(LFH_static));

  char *ptr = reinterpret_cast<char *>(data.data() + sizeof(LFH_static));
  unsigned int length;
  length = this->s.name_length;
  if (length == 0) {
    throw std::runtime_error(
        "Corrupted archive (LFH entry contains no filename)");
  }
  this->name = std::string(ptr, length);
  ptr += length;

  length = this->s.extra_length;
  if (length > 0) {
    this->extra =
        std::vector<std::byte>(reinterpret_cast<std::byte *>(ptr),
                               reinterpret_cast<std::byte *>(ptr + length));
    ptr += length;
  }
  this->data = std::vector<std::byte>(this->s.c_size);
  memcpy(this->data.data(), ptr, this->s.c_size);
}

LFH::LFH(const LFH &old) : Hexdumpable(old.raw_bytes) {
  this->s = old.s;
  this->name = old.name;
  this->extra = old.extra;
  this->data = old.data;
}

LFH::LFH(LFH &&old)
    : Hexdumpable(std::move(old.raw_bytes)), name(std::move(old.name)),
      extra(std::move(old.extra)), data(std::move(old.data)) {
  this->s = old.s;
}

LFH &LFH::operator=(const LFH &old) {
  if (&old == this) {
    return *this;
  }
  this->s = old.s;
  this->raw_bytes = old.raw_bytes;
  this->name = old.name;
  this->extra = old.extra;
  this->data = old.data;
  return *this;
}

CDFH::CDFH() : Hexdumpable() {}

CDFH::CDFH(std::vector<std::byte> &data) : Hexdumpable(data) {
  if (data.size() < sizeof(CDFH_static)) {
    throw std::runtime_error("Corrupted archive (invalid CDFH entry length)");
  }
  memcpy(&this->s, data.data(), sizeof(CDFH_static));

  char *ptr = reinterpret_cast<char *>(data.data() + sizeof(CDFH_static));
  unsigned int length;
  length = this->s.name_length;
  if (length == 0) {
    throw std::runtime_error(
        "Corrupted archive (CDFH entry contains no filename)");
  }
  this->name = std::string(ptr, length);
  ptr += length;

  length = this->s.extra_length;
  if (length > 0) {
    this->extra =
        std::vector<std::byte>(reinterpret_cast<std::byte *>(ptr),
                               reinterpret_cast<std::byte *>(ptr + length));
    ptr += length;
  }
  length = this->s.comment_length;
  if (length > 0) {
    this->comment = std::string(ptr, length);
  }
}

CDFH::CDFH(const CDFH &old) : Hexdumpable(old.raw_bytes) {
  this->s = old.s;
  this->name = old.name;
  this->extra = old.extra;
  this->comment = old.comment;
}

CDFH::CDFH(CDFH &&old)
    : Hexdumpable(std::move(old.raw_bytes)), name(std::move(old.name)),
      extra(std::move(old.extra)), comment(std::move(old.comment)) {
  this->s = old.s;
}

CDFH &CDFH::operator=(const CDFH &old) {
  if (&old == this) {
    return *this;
  }
  this->s = old.s;
  this->raw_bytes = old.raw_bytes;
  this->name = old.name;
  this->extra = old.extra;
  this->comment = old.comment;
  return *this;
}

} // namespace zipcxx