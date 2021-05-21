#include "Zip.hpp"

#include <windows.h>

#include <climits>
#include <cstring>
#include <fstream>

Zip::Zip(const char *filename) {
  this->good = false;
  this->ecdr = nullptr;
  this->raw = nullptr;
  this->rawsz = 0;

  std::ifstream f(filename, std::fstream::binary);
  if (f.good() == false) {
    return;
  }
  f.seekg(0, std::ifstream::end);
  this->rawsz = f.tellg();
  f.seekg(0, std::ifstream::beg);

  this->raw = new unsigned char[this->rawsz];
  f.read((char *)this->raw, this->rawsz);

  // Check if given file size is large enought to hold ECDR
  if (this->rawsz < sizeof(ECDR_base)) {
    return;
  }

  if (Zip::read_ecdr(this->raw, this->rawsz, this->ecdr) == false) {
    return;
  }
  if (Zip::read_cdfhs(this->raw, this->ecdr, this->cdfhs) == false) {
    return;
  }
  if (Zip::read_lfhs(this->raw, this->cdfhs, this->lfhs) == false) {
    return;
  }

  if (this->lfhs.size() != this->cdfhs.size()) {
    // We assume that number of lfs must be the same as the number of cdfh, if
    // it is not we assume it is broken
    return;
  }
  this->good = true;
}

bool Zip::read_ecdr(unsigned char *data, unsigned int data_size, ECDR *&ecdr) {
  if (data == nullptr) {
    return false;
  }
  unsigned char *ptr = data + data_size - sizeof(ECDR_base);

  while (memcmp(ptr, (const void *)&Signature::ECDR, 4) != 0) {
    if (ptr <= data) {
      return false;
    }
    ptr--;
  }
  ecdr = new ECDR(ptr);
  return true;
}

bool Zip::read_cdfhs(unsigned char *data, ECDR *ecdr,
                     std::vector<CDFH *> &cdfhs) {
  if (data == nullptr || ecdr == nullptr) {
    return false;
  }
  unsigned char *ptr = data + ecdr->cd_offset;

  for (size_t i = 0; i < ecdr->number_entries; i++) {
    cdfhs.push_back(new CDFH(ptr));
    CDFH *t = cdfhs[cdfhs.size() - 1];
    ptr += sizeof(CDFH_base) + t->name_length + t->extra_length +
           t->comment_length;
  }
  return true;
}

bool Zip::read_lfhs(unsigned char *data, std::vector<CDFH *> &cdfhs,
                    std::vector<LFH *> &lfhs) {
  if (data == nullptr || cdfhs.size() == 0) {
    return false;
  }
  for (size_t i = 0; i < cdfhs.size(); i++) {
    unsigned char *ptr = data + cdfhs[i]->lh_offset;
    lfhs.push_back(new LFH(ptr));
  }
  return true;
}

/* Checks if entry is directory by checking few characteristic values */
bool Zip::is_directory(CDFH *entry) {
  if (entry->c_method == 0) {
    if (entry->crc32 == 0) {
      if (entry->c_size == 0 && entry->uc_size == 0) {
        if (entry->name_length > 0 &&
            entry->name[entry->name_length - 1] == '/') {
          return true;
        }
      }
    }
  }
  return false;
}

CDFH *Zip::get_cdfh_from_lfh(LFH *lfh) {
  for (size_t i = 0; i < this->lfhs.size(); i++) {
    LFH *tmp = this->lfhs[i];
    if (tmp == lfh) {
      return this->cdfhs[i];
    }
  }
  return nullptr;
}

Zip::~Zip() {
  if (this->raw != nullptr) {
    delete[] this->raw;
  }
  if (this->ecdr != nullptr) {
    delete this->ecdr;
  }
  if (this->good) {
    for (size_t i = 0; i < this->cdfhs.size(); i++) {
      delete this->cdfhs[i];
    }
    for (size_t i = 0; i < this->lfhs.size(); i++) {
      delete this->lfhs[i];
    }
  }
}

/* Prints information about files, flag parameter may be used in the future */
void Zip::list_files(enum Flag f) {
  puts("modification time | file");
  puts("----------------------------------\n");
  for (vector_cdfh::iterator i = this->cdfhs.begin(); i != this->cdfhs.end();
       i++) {
    CDFH *t = *i;
    if (Zip::is_directory(t)) {
      continue;
    }
    DosTime dt(t->mod_time, t->mod_date);
    printf("%02d:%02d %02d-%02d-%d | ", dt.hour, dt.minute, dt.day, dt.month,
           dt.year);

    for (uint16_t j = 0; j < t->name_length; j++) {
      printf("%c", t->name[j]);
    }
    puts("");
  }
}

/* Loops through lfhs and looks for given filename */
LFH *Zip::find_file(const char *filename, std::vector<LFH *> &lfhs) {
  for (vector_lfh::iterator i = lfhs.begin(); i != lfhs.end(); i++) {
    LFH *t = *i;
    if (memcmp((const char *)t->name, filename, strlen(filename)) == 0) {
      return t;
    }
  }
  return nullptr;
}

/* Detects compression method and tries to decompress the archive */
Data *Zip::decompress(LFH *lfh) {
  Data input(lfh->data, lfh->c_size);
  Data *d = nullptr;
  Compression method = (Compression)lfh->c_method;
  if (method == Compression::DEFLATE) {
    d = Utils::zlib_inflate(&input);
  } else if (method == Compression::STORED) {
    d = new Data(lfh->data, lfh->c_size);
  }
  return d;
}

/* Extracts single file from archive */
unsigned int Zip::extract(const char *filename, enum Flag flag) {
  LFH *lfh = Zip::find_file(filename, this->lfhs);
  CDFH *cdfh = Zip::get_cdfh_from_lfh(lfh);
  if (lfh == nullptr) {
    return Status::FILE_NOT_FOUND;
  }

  char *filename_copy = Utils::copy_string(filename);
  char *filename_copy_initial = filename_copy;
  char *dir_status = Utils::find_last_of(filename_copy, "/");

  if (Zip::is_directory(cdfh) == true) {
    Utils::create_directory(filename);
    return Status::OK;
  } else if (dir_status != nullptr) {
    // pointer subtraction to calculate new string size
    size_t dirname_length = (dir_status - filename_copy_initial);

    // copy_string cuts the part of path so it can create directory tree
    // e.g. myfiles/directory1/directory2/secret.txt
    // it must take all before secret.txt and create it
    char *dirname = Utils::copy_string(filename, dirname_length);
    Utils::create_directory(dirname);
    delete[] dirname;
  }
  delete[] filename_copy;

  Data *d = Zip::decompress(lfh);
  if (d == nullptr) {
    return Status::DECOMPRESS_ERROR;
  }

  std::ofstream f(filename, std::ios::binary);
  if (f.good() == false) {
    return Status::IO_ERROR;
  }
  f.write((const char *)d->data, d->data_size);
  f.close();

  return Status::OK;
}

/* Extracts all files from archive */
unsigned int Zip::extract_all(enum Flag flag) {
  for (vector_cdfh::iterator i = this->cdfhs.begin(); i != this->cdfhs.end();
       i++) {
    CDFH *cdfh = *i;
    char *name =
        Utils::copy_string((const char *)cdfh->name, cdfh->name_length);

    unsigned int status = this->extract(name, flag);

    delete[] name;
    if (status != Status::OK) {
      return status;
    }
  }
  return Status::OK;
}

/* Returns true when archive was opened without any errors, false otherwise */
bool Zip::is_good() const { return this->good; }

/* Debugging method used to get hex representation of loaded data */
void Zip::print_data() const {
  for (size_t i = 0; i < this->rawsz; i++) {
    if (i % 16 == 0) {
      puts("");
    }
    printf("%.2x ", this->raw[i]);
  }
}