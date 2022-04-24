#include "Zip.hpp"

#include <algorithm>
#include <climits>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace zipcxx {

Zip::Zip(const std::string &filename) {
  auto rawBytes{utils::readFile(filename, std::ifstream::binary)};
  checkIfValidArchive(rawBytes);

  this->ecdr = Zip::readEcdr(rawBytes);
  auto cdfhs{Zip::readCdfhs(this->ecdr, rawBytes)};
  auto lfhs{Zip::readLfhs(rawBytes, cdfhs)};

  if (not areLoadedHeadersValid(lfhs, cdfhs)) {
    throw std::runtime_error("Corrupted archive");
  }

  this->entries.reserve(cdfhs.size());
  for (unsigned int i = 0; i < cdfhs.size(); i++) {
    this->entries.emplace_back(cdfhs[i], lfhs[i]);
  }
}

bool Zip::isValidLFHSignature(const std::vector<std::byte> &inputSignature) {
  const auto lfhSignature{LFH{}.getSignature()};
  return std::equal(lfhSignature.begin(), lfhSignature.end(),
                    inputSignature.begin(), inputSignature.end(),
                    [](const uint8_t a, const std::byte b) {
                      return (a == static_cast<uint8_t>(b));
                    });
}

bool Zip::areLoadedHeadersValid(const std::vector<LFH> &lfhs,
                                const std::vector<CDFH> &cdfhs) {
  if (lfhs.size() != cdfhs.size()) {
    return false;
  }
  for (unsigned int i = 0; i < cdfhs.size(); i++) {
    const auto &lfh{lfhs[i]};
    const auto &cdfh{cdfhs[i]};

    if (lfh.s.crc32 != cdfh.s.crc32) {
      return false;
    }
  }
  return true;
}

void Zip::checkIfValidArchive(const std::vector<std::byte> &rawBytes) {
  if (rawBytes.size() < 4 or
      not isValidLFHSignature({rawBytes.begin(), rawBytes.begin() + 4})) {
    throw std::runtime_error("Not a valid archive");
  }
  if (rawBytes.size() < sizeof(ECDR_static)) {
    throw std::runtime_error("File truncated");
  }
}

uint32_t Zip::crc32(const std::vector<std::byte> &rawBytes) {
  constexpr unsigned int INITIAL_CRC = 0xFFFFFFFF;
  constexpr unsigned int CRC_POLYNOMIAL = 0xEDB88320;
  constexpr unsigned int BITS_PER_BYTE = 8;

  uint32_t crc = INITIAL_CRC;
  for (unsigned int i = 0; i < rawBytes.size(); i++) {
    char ch = static_cast<char>(rawBytes[i]);
    for (unsigned int j = 0; j < BITS_PER_BYTE; j++) {
      uint32_t b = (ch ^ crc) & 1;
      crc >>= 1;
      if (b != 0U) {
        crc = crc ^ CRC_POLYNOMIAL;
      }
      ch >>= 1;
    }
  }
  return ~crc;
}

bool Zip::areChecksumsValid(const std::vector<std::byte> &rawBytes,
                            const LFH &lfh, const CDFH &cdfh) {
  if (lfh.s.crc32 != cdfh.s.crc32) {
    return false;
  }
  const uint32_t originalChecksum{cdfh.s.crc32};
  return originalChecksum == crc32(rawBytes);
}

std::unique_ptr<ECDR> Zip::readEcdr(const std::vector<std::byte> &data) {
  if (data.empty()) {
    throw std::runtime_error("Couldn't read ECDR structure");
  }
  const std::byte *ptr = data.data() + data.size() - sizeof(ECDR_static);
  auto ecdrSignature{ECDR{}.getSignature()};
  while (memcmp(ptr, (const void *)&ecdrSignature, 4) != 0) {
    if (ptr <= data.data()) {
      throw std::runtime_error("Invalid ZIP file");
    }
    ptr--;
  }
  const std::byte *begin = ptr;
  const std::byte *end = data.data() + data.size();

  std::vector<std::byte> ecdr(begin, end);
  return std::make_unique<ECDR>(ecdr);
}

std::vector<CDFH> Zip::readCdfhs(const std::unique_ptr<ECDR> &ecdr,
                                 const std::vector<std::byte> &data) {
  std::vector<CDFH> cdfhs;
  if (data.empty() or ecdr == nullptr) {
    throw std::runtime_error("Couldn't read CDFHs structures");
  }

  const std::byte *begin = data.data() + ecdr->s.cd_offset;
  const std::byte *end = nullptr;
  for (size_t i = 0; i < ecdr->s.number_entries; i++) {
    const CDFH_static *s = reinterpret_cast<const CDFH_static *>(begin);
    end = begin + sizeof(CDFH_static) + s->name_length + s->extra_length +
          s->comment_length;
    std::vector<std::byte> cdfh(begin, end);
    cdfhs.emplace_back(cdfh);
    begin = end;
  }
  return cdfhs;
}

std::vector<LFH> Zip::readLfhs(const std::vector<std::byte> &data,
                               const std::vector<CDFH> &cdfhs) {
  std::vector<LFH> lfhs;
  if (data.empty() or cdfhs.empty()) {
    throw std::runtime_error("Couldn't read LFHs structures");
  }
  int i = 0;
  for (const CDFH &cdfh : cdfhs) {
    const std::byte *begin = data.data() + cdfh.s.lh_offset;
    const std::byte *end = begin + sizeof(LFH_static);
    const LFH_static *s = reinterpret_cast<const LFH_static *>(begin);
    end += s->name_length + s->extra_length + cdfh.s.c_size;

    std::vector<std::byte> lfh(begin, end);
    lfhs.emplace_back(lfh);
    i++;
  }
  return lfhs;
}

/* Checks if entry is directory by checking few characteristic values */
bool Zip::isDirectory(const CDFH &entry) {
  if (entry.s.c_method == 0) {
    if (entry.s.crc32 == 0) {
      if (entry.s.c_size == 0 && entry.s.uc_size == 0) {
        if (entry.s.name_length > 0 && entry.name.back() == '/') {
          return true;
        }
      }
    }
  }
  return false;
}

std::vector<std::string> Zip::getFilenames() const {
  std::vector<std::string> filenames{};
  for (const auto &e : this->entries) {
    filenames.push_back(e.getFilename());
  }
  return filenames;
}

/* Detects compression method and tries to decompress the archive */
std::vector<std::byte> Zip::decompress(LFH &lfh) {
  std::vector<std::byte> output;
  Compression method = static_cast<Compression>(lfh.s.c_method);
  if (method == Compression::DEFLATE) {
    output = utils::zlib_inflate(lfh.data);
  } else if (method == Compression::STORED) {
    output.resize(lfh.s.c_size);
    memcpy(output.data(), lfh.data.data(), lfh.data.size());
  }
  return output;
}

ZipEntry &Zip::getEntryByFilename(const std::string &filename) {
  auto it = std::find_if(
      this->entries.begin(), this->entries.end(),
      [&filename](const auto &e) { return (e.getFilename() == filename); });

  if (it == this->entries.end()) {
    throw std::runtime_error("Given file doesn't exist in archive");
  }
  return *it;
}

/* Extracts single file from archive */
void Zip::extract(const std::string &filename) {
  ZipEntry &e = this->getEntryByFilename(filename);
  CDFH &cdfh = e.getCdfh();
  LFH &lfh = e.getLfh();

  size_t dirname_length = filename.find_last_of('/');
  if (Zip::isDirectory(cdfh)) {
    std::filesystem::create_directories(filename);
    return;
  }
  if (dirname_length != std::string::npos) {
    std::string dirname = filename.substr(0, dirname_length);
    if (not dirname.empty()) {
      std::filesystem::create_directories(filename);
    }
  }

  std::vector<std::byte> data = Zip::decompress(lfh);
  if (not areChecksumsValid(data, lfh, cdfh)) {
    throw std::runtime_error("Invalid checksum");
  }
  utils::writeFile(filename, data);
}

/* Extracts all files from archive */
void Zip::extractAll() {
  for (auto &entry : this->entries) {
    CDFH &c_cdfh = entry.getCdfh();
    std::string name = entry.getFilename();
    this->extract(name);
  }
}

std::vector<ZipEntry> &Zip::getEntries() { return this->entries; }
} // namespace zipcxx