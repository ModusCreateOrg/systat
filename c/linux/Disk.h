#ifndef DISK_H
#define DISK_H

#include "systat.h"

struct DiskStats {
  const char *name;
  uint64_t major, minor;
  uint64_t reads_issued, reads_merged, sectors_read, time_reading,
      writes_completed, writes_merged, sectors_written, time_writing,
      ios_in_progress, time_doing_ios, weighted_time_ios;

public:
  void diff(DiskStats *newer, DiskStats *older);
};

class Disk {
  std::map<std::string, DiskStats *> last, current, delta;

public:
  uint16_t num_disks;

public:
  Disk();
  ~Disk();

private:
  uint16_t read(std::map<std::string, DiskStats *> &s);
  void copy(std::map<std::string, DiskStats *> &dst,
            std::map<std::string, DiskStats *> &src);

public:
  void update();
  void print();
};

extern Disk disk;

#endif
