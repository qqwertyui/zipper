#include "Zip_structs.hpp"

#include <zlib.h>

#include <cstdio>
#include <cstring>

ECDR::ECDR(unsigned char *data) {
  memcpy(this, data, sizeof(ECDR_base));

  this->comment = nullptr;
  if (this->comment_length > 0) {
    this->comment = new unsigned char[this->comment_length];
    memcpy(this->comment, data + sizeof(ECDR_base), this->comment_length);
  }
}

ECDR::~ECDR() {
  if (this->comment != nullptr) {
    delete[] this->comment;
  }
}

LFH::LFH(unsigned char *data) {
  memcpy(this, data, sizeof(LFH_base));

  this->name = this->extra = nullptr;
  size_t length, offset = 0;

  length = this->name_length;
  if (length > 0) {
    this->name = new unsigned char[length];
    memcpy(this->name, data + sizeof(LFH_base), length);
    offset += length;
  }
  length = this->extra_length;
  if (length > 0) {
    this->extra = new unsigned char[length];
    memcpy(this->extra, data + sizeof(LFH_base) + offset, length);
    offset += length;
  }
  this->data = new unsigned char[this->c_size];  // if c_size equals 0 it will
                                                 // succeed anyway
  memcpy(this->data, data + sizeof(LFH_base) + offset, this->c_size);
}

LFH::~LFH() {
  if (this->name != nullptr) {
    delete[] this->name;
  }
  if (this->extra != nullptr) {
    delete[] this->extra;
  }

  delete[] this->data;
}

CDFH::CDFH(unsigned char *data) {
  memcpy(this, data, sizeof(CDFH_base));

  this->comment = this->extra = this->name = nullptr;
  size_t length, offset = 0;

  length = this->name_length;
  if (length > 0) {
    this->name = new unsigned char[length];
    memcpy(this->name, data + sizeof(CDFH_base), length);
    offset += length;
  }

  length = this->extra_length;
  if (length > 0) {
    this->extra = new unsigned char[length];
    memcpy(this->extra, data + sizeof(CDFH_base) + offset, length);
    offset += length;
  }

  length = this->comment_length;
  if (length > 0) {
    this->comment = new unsigned char[length];
    memcpy(this->comment, data + sizeof(CDFH_base) + offset, length);
  }
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