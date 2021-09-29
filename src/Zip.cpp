#include "Zip.hpp"

#include <windows.h>

#include <climits>
#include <cstring>
#include <fstream>

Zip::Zip(std::string filename) {
  std::vector<std::byte> raw = Utils::read_file(filename, std::ifstream::binary);

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

std::unique_ptr<ECDR> Zip::read_ecdr(std::vector<std::byte> &data) {
  if (data.size() == 0) {
    throw std::runtime_error("Couldn't read ECDR structure");
  }
  std::byte *ptr = data.data() + data.size() - sizeof(ECDR_base);
  while (memcmp(ptr, (const void *)&Signature::ECDR, 4) != 0) {
    if (ptr <= data.data()) {
      throw std::runtime_error("Unsupported/broken ECDR");
    }
    ptr--;
  }
  return std::make_unique<ECDR>(ptr);
}

std::vector<CDFH*> Zip::read_cdfhs(std::vector<std::byte> &data) {
  std::vector<CDFH*> cdfhs;
  if (data.size() == 0 || this->ecdr == nullptr) {
    throw std::runtime_error("Couldn't read CDFHs structures");
  }
  std::byte *ptr = data.data() + this->ecdr->cd_offset;

  for (size_t i = 0; i < this->ecdr->number_entries; i++) {
    CDFH *t = new CDFH(ptr);
    cdfhs.push_back(t);
    ptr += sizeof(CDFH_base) + t->name_length + t->extra_length +
           t->comment_length;
  }
  return cdfhs;
}

std::vector<LFH*> Zip::read_lfhs(std::vector<std::byte> &data, std::vector<CDFH*> &cdfhs) {
  std::vector<LFH*> lfhs;
  if (data.size() == 0 || cdfhs.size() == 0) {
    throw std::runtime_error("Couldn't read LFHs structures");
  }
  for(CDFH *c_cdfh : cdfhs) {
    std::byte *ptr = data.data() + c_cdfh->lh_offset;
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

/* Detects compression method and tries to decompress the archive */
std::vector<std::byte> Zip::decompress(LFH *lfh) {
  std::vector<std::byte> output;
  Data input(lfh->data, lfh->c_size);
  Compression method = (Compression)lfh->c_method;
  if (method == Compression::DEFLATE) {
    output = Utils::zlib_inflate(&input);
  } else if (method == Compression::STORED) {
    output.resize(lfh->c_size);
    memcpy(output.data(), lfh->data, lfh->c_size);
  }
  return output;
}

ZipEntry* Zip::get_entry_by_filename(std::string &filename) {
    for(ZipEntry *e : this->entries) {
        if(e->get_name().compare(filename) == 0) {
            return e;
        }
    }
    return nullptr;
}

/* Extracts single file from archive */
void Zip::extract(std::string &filename) {
  ZipEntry *e = this->get_entry_by_filename(filename);
  CDFH &cdfh = e->get_cdfh();
  LFH &lfh = e->get_lfh();
  if (e == nullptr) {
    throw std::runtime_error("Couldn't find given file");
  }

  std::string filename_copy = filename;
  char *nested_directory = Utils::find_last_of(filename_copy.data(), "/");
  char *filename_copy_initial = filename_copy.data();

  if (Zip::is_directory(&cdfh) == true) {
    Utils::create_directory(filename);
    return;
  } else if (nested_directory != nullptr) {
    // pointer subtraction to calculate new string size
    size_t dirname_length = (nested_directory - filename_copy_initial);

    // split path and basename
    // e.g. myfiles/directory1/directory2/secret.txt
    // path: myfiles/directory1/directory2/ basename: secret.txt
    // then, create path and put the file in it
    std::string dirname = filename.substr(0, dirname_length);
    Utils::create_directory(dirname);
  }

  std::vector<std::byte> data = Zip::decompress(&lfh);
  Utils::write_file(filename, data);
}

/* Extracts all files from archive */
void Zip::extract_all() {
  for(ZipEntry *entry : this->entries) {
    CDFH &c_cdfh = entry->get_cdfh();
    std::string name = entry->get_name();

    printf("Extracting %s\n", name.c_str());
    this->extract(name);
  }
}

std::vector<ZipEntry*> Zip::get_entries() const {
    return this->entries;
}

ZipEntry::ZipEntry(const CDFH &cdfh, const LFH &lfh) {
    this->cdfh = cdfh;
    this->lfh = lfh;

    this->filename = std::string(cdfh.name, cdfh.name_length);
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