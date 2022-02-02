#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
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

/*
  Local File Header
*/
class LFH {
public:
  LFH() = default;
  LFH(std::vector<std::byte> &data);
  LFH(const LFH &old);
  ~LFH();

  LFH &operator=(const LFH &old);

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

  char *name = nullptr;
  char *extra = nullptr;
  std::byte *data = nullptr;

  static constexpr int FIXED_FIELDS_LENGTH = 30;
} __attribute__((packed));

/*
  Central Directory File Header
*/
class CDFH {
public:
  CDFH() = default;
  CDFH(std::vector<std::byte> &data);
  ~CDFH();

  CDFH &operator=(const CDFH &old);

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

  char *name = nullptr;
  char *extra = nullptr;
  char *comment = nullptr;

  static constexpr int FIXED_FIELDS_LENGTH = 46;
} __attribute__((packed));

/*
  End of Central Directory Record
*/
class ECDR {
public:
  ECDR(std::vector<std::byte> &data);
  ~ECDR();

  std::byte sig[4];
  uint16_t disk_num;
  uint16_t disk_num_cdfh;
  uint16_t number_entries_disk;
  uint16_t number_entries;
  uint32_t size_cdh;
  uint32_t cd_offset;
  uint16_t comment_length;

  char *comment = nullptr;

  static constexpr int FIXED_FIELDS_LENGTH = 22;
} __attribute__((packed));

namespace Signature {
constexpr unsigned int LFH = 0x04034b50;
constexpr unsigned int CDFH = 0x02014b50;
constexpr unsigned int ECDR = 0x06054b50;
}; // namespace Signature