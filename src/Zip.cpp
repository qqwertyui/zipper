#include "Zip.hpp"

#include <windows.h>

#include <climits>
#include <cstring>
#include <fstream>

Zip::Zip(std::string filename) {
  std::vector<unsigned char> raw = Utils::read_file(filename, std::ifstream::binary);

  // Check if given file size is large enought to hold ECDR
  if (raw.size() < sizeof(ECDR_base)) {
    throw std::runtime_error("Parsing error");
  }

  this->ecdr = Zip::read_ecdr(raw);
  std::vector<CDFH*> cdfhs = Zip::read_cdfhs(raw);
  std::vector<LFH*> lfhs = Zip::read_lfhs(raw, cdfhs);

  if (lfhs.size() != cdfhs.size()) {
    // We assume that number of lfs must be the same as the number of cdfh,
    // if it is not we assume it is broken
    throw std::runtime_error("Corrupted archive");
  }

  for(unsigned int i=0; i<cdfhs.size(); i++) {
    this->entries.push_back(new ZipEntry(*cdfhs[i], *lfhs[i]));
  }
}

std::unique_ptr<ECDR> Zip::read_ecdr(std::vector<unsigned char> &data) {
  if (data.size() == 0) {
    throw std::runtime_error("Couldn't read ECDR structure");
  }
  unsigned char *ptr = data.data() + data.size() - sizeof(ECDR_base);
  while (memcmp(ptr, (const void *)&Signature::ECDR, 4) != 0) {
    if (ptr <= data.data()) {
      throw std::runtime_error("Unsupported/broken ECDR");
    }
    ptr--;
  }
  return std::make_unique<ECDR>(ptr);
}

std::vector<CDFH*> Zip::read_cdfhs(std::vector<unsigned char> &data) {
  std::vector<CDFH*> cdfhs;
  if (data.size() == 0 || this->ecdr == nullptr) {
    throw std::runtime_error("Couldn't read CDFHs structures");
  }
  unsigned char *ptr = data.data() + this->ecdr->cd_offset;

  for (size_t i = 0; i < this->ecdr->number_entries; i++) {
    CDFH *t = new CDFH(ptr);
    cdfhs.push_back(t);
    ptr += sizeof(CDFH_base) + t->name_length + t->extra_length +
           t->comment_length;
  }
  return cdfhs;
}

std::vector<LFH*> Zip::read_lfhs(std::vector<unsigned char> &data, std::vector<CDFH*> &cdfhs) {
  std::vector<LFH*> lfhs;
  if (data.size() == 0 || cdfhs.size() == 0) {
    throw std::runtime_error("Couldn't read LFHs structures");
  }
  for(CDFH *c_cdfh : cdfhs) {
    unsigned char *ptr = data.data() + c_cdfh->lh_offset;
    lfhs.push_back(new LFH(ptr));
  }
  return lfhs;
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

Zip::~Zip() {
  for(ZipEntry *e : this->entries) {
      delete e;
  }
}

/* Prints information about files, flag parameter may be used in the future */
void Zip::list_files() const {
  puts("modification time | file");
  puts("----------------------------------");
  for(ZipEntry *e : this->entries) {
    const DosTime &dt = e->get_time();
    printf("%02d:%02d %02d-%02d-%d | ", dt.hour, dt.minute, dt.day, dt.month, dt.year);
    printf("%s\n", e->get_name().c_str());
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

ZipEntry* Zip::get_entry_by_filename(std::string filename) {
    for(ZipEntry *e : this->entries) {
        if(e->get_name().compare(filename) == 0) {
            return e;
        }
    }
    return nullptr;
}

/* Extracts single file from archive */
unsigned int Zip::extract(const char *filename) {
  ZipEntry *e = this->get_entry_by_filename(filename);
  CDFH &cdfh = e->get_cdfh();
  LFH &lfh = e->get_lfh();
  if (e == nullptr) {
    return Status::FILE_NOT_FOUND;
  }

  std::unique_ptr<char[]> filename_copy = Utils::copy_string(filename);
  char *dir_status = Utils::find_last_of(filename_copy.get(), "/");
  char *filename_copy_initial = filename_copy.get();

    // change to reference
  if (Zip::is_directory(&cdfh) == true) {
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

  Data *d = Zip::decompress(&lfh);
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
  for(ZipEntry *entry : this->entries) {
    CDFH &c_cdfh = entry->get_cdfh();
    std::unique_ptr<char[]> name = Utils::copy_string((const char *)c_cdfh.name, c_cdfh.name_length);

    printf("Extracting %s\n", name.get());
    unsigned int status = this->extract(name.get());

    if (status != Status::OK) {
      return status;
    }
  }
  return Status::OK;
}

std::vector<ZipEntry*> Zip::get_entries() const {
    return this->entries;
}

ZipEntry::ZipEntry(const CDFH &cdfh, const LFH &lfh) {
    this->cdfh = cdfh;
    this->lfh = lfh;

    this->filename = Utils::copy_string((const char*)cdfh.name, cdfh.name_length).get();
    this->time = new DosTime(cdfh.mod_time, cdfh.mod_date);
}

CDFH& ZipEntry::get_cdfh() {
    return this->cdfh;
}

LFH& ZipEntry::get_lfh() {
    return this->lfh;
}

ZipEntry::~ZipEntry() {
    delete this->time;
}

const std::string& ZipEntry::get_name() const {
    return this->filename;
}

const DosTime& ZipEntry::get_time() const {
    return *this->time;
}