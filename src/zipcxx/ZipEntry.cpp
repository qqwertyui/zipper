#include "ZipEntry.hpp"
#include "ZipStructs.hpp"
#include "utils/Utils.hpp"
#include <iomanip>
#include <sstream>
#include <utility>

namespace zipcxx {

ZipEntry::ZipEntry(const ZipEntry &old)
    : cdfh(std::make_unique<CDFH>(*old.cdfh)),
      lfh(std::make_unique<LFH>(*old.lfh)),
      time(std::make_unique<utils::DosTime>((uint16_t)old.cdfh->s.mod_time,
                                            (uint16_t)old.cdfh->s.mod_date)) {}

ZipEntry::ZipEntry(const CDFH &cdfh, const LFH &lfh)
    : cdfh(std::make_unique<CDFH>(cdfh)), lfh(std::make_unique<LFH>(lfh)),
      time(std::make_unique<utils::DosTime>(cdfh.s.mod_time, cdfh.s.mod_date)) {
}

ZipEntry::ZipEntry(ZipEntry &&old) noexcept
    : cdfh(std::move(old.cdfh)), lfh(std::move(old.lfh)),
      time(std::move(old.time)) {}

CDFH &ZipEntry::getCdfh() { return *cdfh; }
const CDFH &ZipEntry::getCdfh() const { return *cdfh; }

LFH &ZipEntry::getLfh() { return *lfh; }
const LFH &ZipEntry::getLfh() const { return *lfh; }

std::string ZipEntry::getFilename() const { return cdfh->name; }

std::string ZipEntry::getTimeAsString(const std::string &format) const {
  auto timeinfo{time->getTmStruct()};
  std::stringstream buffer;
  buffer << std::put_time(&timeinfo, format.c_str());
  return buffer.str();
}

utils::DosTime &ZipEntry::getTime() { return *time; }

} // namespace zipcxx