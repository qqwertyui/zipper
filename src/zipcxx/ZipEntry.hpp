#ifndef ZIPENTRY_HPP
#define ZIPENTRY_HPP

#include <memory>
#include <string>

namespace zipcxx {
class CDFH;
class LFH;

namespace utils {
class DosTime;
}

class ZipEntry {
public:
  explicit ZipEntry(const CDFH &cdfh, const LFH &lfh);
  ZipEntry(ZipEntry &&old) noexcept;
  ZipEntry(const ZipEntry &old);

  std::string getFilename() const;
  std::string
  getTimeAsString(const std::string &format = "%a %b %d %H:%M:%S %Y") const;

  utils::DosTime &getTime();
  CDFH &getCdfh();
  const CDFH &getCdfh() const;

  LFH &getLfh();
  const LFH &getLfh() const;

private:
  std::unique_ptr<utils::DosTime> time;
  std::unique_ptr<CDFH> cdfh;
  std::unique_ptr<LFH> lfh;
};
} // namespace zipcxx

#endif