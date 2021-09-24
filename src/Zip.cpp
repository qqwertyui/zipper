#include "Zip.hpp"

#include <windows.h>

#include <climits>
#include <cstring>
#include <fstream>

Zip::Zip(std::string filename) {
  std::ifstream file(filename, std::fstream::binary);
  if (file.good() == false) {
    throw std::runtime_error("Unable to open the file");
  }
  file.seekg(0, std::ifstream::end);
  this->raw.resize(file.tellg());
  file.seekg(0, std::ifstream::beg);

  file.read((char*)this->raw.data(), this->raw.size());
  file.close();

  // Check if given file size is large enought to hold ECDR
  if (this->raw.size() < sizeof(ECDR_base)) {
    throw std::runtime_error("Parsing error");
  }

  if (Zip::read_ecdr(this->raw) == false) {
    throw std::runtime_error("Couldn't read ECDR structure");
  }
  if (Zip::read_cdfhs(this->raw) == false) {
    throw std::runtime_error("Couldn't read CDFHS structures");
  }
  if (Zip::read_lfhs(this->raw) == false) {
    throw std::runtime_error("Couldn't read lfhs structures");
  }

  if (this->lfhs.size() != this->cdfhs.size()) {
    // We assume that number of lfs must be the same as the number of cdfh,
    // if it is not we assume it is broken
    return;
  }
}

bool Zip::read_ecdr(std::vector<unsigned char> &data) {
  if (data.size() == 0) {
    return false;
  }
  unsigned char *ptr = data.data() + data.size() - sizeof(ECDR_base);

  while (memcmp(ptr, (const void *)&Signature::ECDR, 4) != 0) {
    if (ptr <= data.data()) {
      return false;
    }
    ptr--;
  }
  this->ecdr = std::make_unique<ECDR>(ptr);
  return true;
}

bool Zip::read_cdfhs(std::vector<unsigned char> &data) {
  if (data.size() == 0 || this->ecdr == nullptr) {
    return false;
  }
  unsigned char *ptr = data.data() + this->ecdr->cd_offset;

  for (size_t i = 0; i < this->ecdr->number_entries; i++) {
    CDFH *t = new CDFH(ptr);
    this->cdfhs.push_back(t);
    ptr += sizeof(CDFH_base) + t->name_length + t->extra_length +
           t->comment_length;
  }
  return true;
}

bool Zip::read_lfhs(std::vector<unsigned char> &data) {
  if (data.size() == 0 || this->cdfhs.size() == 0) {
    return false;
  }
  for(CDFH *c_cdfh : this->cdfhs) {
    unsigned char *ptr = data.data() + c_cdfh->lh_offset;
    this->lfhs.push_back(new LFH(ptr));
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
  for(CDFH *c : this->cdfhs) {
      delete c;
  }
  for(LFH *c : this->lfhs) {
      delete c;
  }
}

/* Prints information about files, flag parameter may be used in the future */
void Zip::list_files() {
  puts("modification time | file");
  puts("----------------------------------");
  for(CDFH *c_cdfh : this->cdfhs) {
    if (Zip::is_directory(c_cdfh)) {
      continue;
    }
    DosTime dt(c_cdfh->mod_time, c_cdfh->mod_date);
    printf("%02d:%02d %02d-%02d-%d | ", dt.hour, dt.minute, dt.day, dt.month,
           dt.year);

    for (uint16_t j = 0; j < c_cdfh->name_length; j++) {
      printf("%c", c_cdfh->name[j]);
    }
    puts("");
  }
}

/* Loops through lfhs and looks for given filename */
LFH *Zip::find_file(const char *filename, std::vector<LFH *> &lfhs) {
  for(LFH *c_lfh : lfhs) {
    if (memcmp((const char *)c_lfh->name, filename, strlen(filename)) == 0) {
      return c_lfh;
    }
  }
  return nullptr;
}

/* Detects compression method and tries to decompress the archive */
Data* Zip::decompress(LFH *lfh) {
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
unsigned int Zip::extract(const char *filename) {
  LFH *lfh = Zip::find_file(filename, this->lfhs);
  CDFH *cdfh = Zip::get_cdfh_from_lfh(lfh);
  if (lfh == nullptr) {
    return Status::FILE_NOT_FOUND;
  }

  std::unique_ptr<char[]> filename_copy = Utils::copy_string(filename);
  char *dir_status = Utils::find_last_of(filename_copy.get(), "/");
  char *filename_copy_initial = filename_copy.get();

  if (Zip::is_directory(cdfh) == true) {
    Utils::create_directory(filename);
    return Status::OK;
  } else if (dir_status != nullptr) {
    // pointer subtraction to calculate new string size
    size_t dirname_length = (dir_status - filename_copy_initial);

    // copy_string cuts the part of path so it can create directory tree
    // e.g. myfiles/directory1/directory2/secret.txt
    // it must take all before secret.txt and create it
    std::unique_ptr<char[]> dirname = Utils::copy_string(filename, dirname_length);
    Utils::create_directory(dirname.get());
  }

  Data *d = Zip::decompress(lfh);
  if (d == nullptr) {
    return Status::DECOMPRESS_ERROR;
  }

  std::ofstream file(filename, std::ios::binary);
  if (file.good() == false) {
    return Status::IO_ERROR;
  }
  file.write((const char *)d->data, d->data_size);
  file.close();

  return Status::OK;
}

/* Extracts all files from archive */
unsigned int Zip::extract_all() {
  for(CDFH *c_cdfh : this->cdfhs) {
    std::unique_ptr<char[]> name = Utils::copy_string((const char *)c_cdfh->name, c_cdfh->name_length);

    printf("Extracting %s\n", name.get());
    unsigned int status = this->extract(name.get());

    if (status != Status::OK) {
      return status;
    }
  }
  return Status::OK;
}