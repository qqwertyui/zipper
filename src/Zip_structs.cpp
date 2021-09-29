#include "Zip_structs.hpp"

#include <cstring>
#include <zlib.h>
#include <cstdio>

ECDR::ECDR(std::vector<std::byte> &data) {
  memcpy(this, data.data(), ECDR::FIXED_FIELDS_LENGTH);

  if (this->comment_length > 0) {
    this->comment = new char[this->comment_length];
    memcpy(this->comment, data.data() + ECDR::FIXED_FIELDS_LENGTH, this->comment_length);
  }
}

ECDR::~ECDR() {
  if (this->comment != nullptr) {
    delete[] this->comment;
  }
}

LFH::LFH(std::vector<std::byte> &data) {
  memcpy(this, data.data(), LFH::FIXED_FIELDS_LENGTH);

  unsigned int length, offset = 0;
  length = this->name_length;
  if (length > 0) {
    this->name = new char[length];
    memcpy(this->name, data.data() + LFH::FIXED_FIELDS_LENGTH, length);
    offset += length;
  }
  length = this->extra_length;
  if (length > 0) {
    this->extra = new char[length];
    memcpy(this->extra, data.data() + LFH::FIXED_FIELDS_LENGTH + offset, length);
    offset += length;
  }
  this->data = new std::byte[this->c_size];
  memcpy(this->data, data.data() + LFH::FIXED_FIELDS_LENGTH + offset, this->c_size);
}

LFH& LFH::operator=(const LFH &old) {
  memcpy(this, &old, LFH::FIXED_FIELDS_LENGTH);

  if(old.name != nullptr) {
    this->name = new char[old.name_length];
    memcpy(this->name, old.name, old.name_length);
  } if(old.extra != nullptr) {
    this->extra = new char[old.extra_length];
    memcpy(this->extra, old.extra, old.extra_length);
  }
  this->data = new std::byte[old.c_size];
  memcpy(this->data, old.data, old.c_size);
  return *this;
}

LFH::~LFH() {
  if (this->name != nullptr) {
    delete[] this->name;
  }
  if (this->extra != nullptr) {
    delete[] this->extra;
  }
  if(this->data != nullptr) {
    delete[] this->data;
  }
}

CDFH::CDFH(std::vector<std::byte> &data) {
  memcpy(this, data.data(), CDFH::FIXED_FIELDS_LENGTH);

  unsigned int length, offset = 0;
  length = this->name_length;
  if (length > 0) {
    this->name = new char[length];
    memcpy(this->name, data.data() + CDFH::FIXED_FIELDS_LENGTH, length);
    offset += length;
  }
  length = this->extra_length;
  if (length > 0) {
    this->extra = new char[length];
    memcpy(this->extra, data.data() + CDFH::FIXED_FIELDS_LENGTH + offset, length);
    offset += length;
  }
  length = this->comment_length;
  if (length > 0) {
    this->comment = new char[length];
    memcpy(this->comment, data.data() + CDFH::FIXED_FIELDS_LENGTH + offset, length);
  }
}

CDFH& CDFH::operator=(const CDFH &old) {
  memcpy(this, &old, CDFH::FIXED_FIELDS_LENGTH);

  if(old.name != nullptr) {
    this->name = new char[old.name_length];
    memcpy(this->name, old.name, old.name_length);
  } if(old.extra != nullptr) {
    this->extra = new char[old.extra_length];
    memcpy(this->extra, old.extra, old.extra_length);
  } if(old.comment != nullptr) {
    this->comment = new char[old.comment_length];
    memcpy(this->comment, old.comment, old.comment_length);
  }
  return *this;
}


CDFH::~CDFH() {
  if (this->name != nullptr) {
    delete[] this->name;
  }
  if (this->extra != nullptr) {
    delete[] this->extra;
  }
  if (this->comment != nullptr) {
    delete[] this->comment;
  }
}