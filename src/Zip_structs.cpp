#include "Zip_structs.hpp"

#include <cstring>
#include <zlib.h>

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
  this->data = new unsigned char[this->c_size]; // if c_size equals 0 it will
                                                // succeed anyway
  memcpy(this->data, data + sizeof(LFH_base) + offset, this->c_size);
}

LFH& LFH::operator=(const LFH &old) {
    memcpy(this, &old, sizeof(LFH_base));
    if(old.name != nullptr) {
        this->name = new unsigned char[old.name_length];
        memcpy(this->name, old.name, old.name_length);
    } if(old.extra != nullptr) {
        this->extra = new unsigned char[old.extra_length];
        memcpy(this->extra, old.extra, old.extra_length);
    }
    this->data = new unsigned char[old.c_size];
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

CDFH& CDFH::operator=(const CDFH &old) {
    memcpy(this, &old, sizeof(CDFH_base));
    
    if(old.name != nullptr) {
        this->name = new unsigned char[old.name_length];
        memcpy(this->name, old.name, old.name_length);
    } if(old.extra != nullptr) {
        this->extra = new unsigned char[old.extra_length];
        memcpy(this->extra, old.name, old.extra_length);
    } if(old.comment != nullptr) {
        this->comment = new unsigned char[old.comment_length];
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