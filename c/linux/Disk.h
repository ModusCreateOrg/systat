#ifndef DISK_H
#define DISK_H

#include "systat.h"

struct DiskStats {
  const char *name;
  unsigned long major, minor;
  unsigned long reads_issued, reads_merged, sectors_read, time_reading,
      writes_completed, writes_merged, sectors_written, time_writing,
      ios_in_progress, time_doing_ios, weighted_time_ios;

public:
  void diff(DiskStats *newer, DiskStats *older);
};

class Disk {
  std::map<std::string, DiskStats *> last, current, delta;

public:
  int num_disks;

public:
  Disk();
  ~Disk();

private:
  int read(std::map<std::string, DiskStats *> &s);
  void copy(std::map<std::string, DiskStats *> &dst,
            std::map<std::string, DiskStats *> &src);

public:
  void update();
  void print();
};

extern Disk disk;

#endif
