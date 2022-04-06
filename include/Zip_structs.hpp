#pragma once

#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>


/*
Taken directly from APPNOTE.TXT, but is not used at all

enum class gpbf {
        encrypted = 		(1 << 0),
        encoding1 = 		(1 << 1),
        encoding2 = 		(1 << 2),
        values = 			(1 << 3),
        enh_deflate =  		(1 << 4),
        comp_patched = 		(1 << 5),
        strong_encrypt =	(1 << 6),
        unused1 = 			(1 << 7),
        unused2 = 			(1 << 8),
        unused3 = 			(1 << 9),
        unused4 = 			(1 << 10),
        lang_encoding = 	(1 << 11),
        reserved1 = 		(1 << 12),
        hidden = 			(1 << 13),
        reserved2 = 		(1 << 14),
        reserved3 = 		(1 << 15)
};
*/

namespace zipper {

class Hexdumpable {
public:
  Hexdumpable() = default;
  Hexdumpable(const std::vector<std::byte> &raw_bytes);
  Hexdumpable(const Hexdumpable &old);
  Hexdumpable(Hexdumpable &&old);

  virtual void hexdump();

protected:
  std::vector<std::byte> raw_bytes;
};

class Signaturable {
public:
  using Type = std::array<uint8_t, 4>;
  virtual Type getSignature() = 0;
};

struct LFH_static {
  std::byte sig[4];
  uint16_t ex_version;
  uint16_t gpbf;
  uint16_t c_method;
  uint16_t mod_time;
  uint16_t mod_date;
  uint32_t crc32;
  uint32_t c_size;
  uint32_t uc_size;
  uint16_t name_length;
  uint16_t extra_length;
} __attribute__((packed));

/*
  Local File Header
*/
class LFH : public Signaturable, Hexdumpable {
public:
  LFH();
  LFH(std::vector<std::byte> &data);
  LFH(const LFH &old);
  LFH(LFH &&old);
  LFH &operator=(const LFH &old);

  LFH_static s;
  std::string name;
  std::optional<std::string> extra;
  std::vector<std::byte> data;

  Signaturable::Type getSignature() override {
    return Signaturable::Type{0x50, 0x4b, 0x03, 0x04};
  }
};

/*
  Central Directory File Header
*/
struct CDFH_static {
  std::byte sig[4];
  uint16_t version_made;
  uint16_t version_needed;
  uint16_t gpbf;
  uint16_t c_method;
  uint16_t mod_time;
  uint16_t mod_date;
  uint32_t crc32;
  uint32_t c_size;
  uint32_t uc_size;
  uint16_t name_length;
  uint16_t extra_length;
  uint16_t comment_length;
  uint16_t disk_num;
  uint16_t int_attributes;
  uint32_t ext_attributes;
  uint32_t lh_offset;
} __attribute__((packed));

class CDFH : public Signaturable, Hexdumpable {
public:
  CDFH();
  CDFH(std::vector<std::byte> &data);
  CDFH(const CDFH &old);
  CDFH(CDFH &&old);
  CDFH &operator=(const CDFH &old);

  CDFH_static s;
  std::string name;
  std::optional<std::string> extra;
  std::optional<std::string> comment;

  Signaturable::Type getSignature() override {
    return Signaturable::Type{0x50, 0x4b, 0x01, 0x02};
  }
};

/*
  End of Central Directory Record
*/
struct ECDR_static {
  std::byte sig[4];
  uint16_t disk_num;
  uint16_t disk_num_cdfh;
  uint16_t number_entries_disk;
  uint16_t number_entries;
  uint32_t size_cdh;
  uint32_t cd_offset;
  uint16_t comment_length;
} __attribute__((packed));

class ECDR : public Signaturable, Hexdumpable {
public:
  ECDR();
  ECDR(std::vector<std::byte> &data);

  ECDR_static s;
  std::optional<std::string> comment;

  Signaturable::Type getSignature() override {
    return Signaturable::Type{0x50, 0x4b, 0x05, 0x06};
  }
};

} // namespace zipper