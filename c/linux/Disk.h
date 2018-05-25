/*
 * systat for Linux
 *
 * Programmed by Mike Schwartz <mike@moduscreate.com>
 *
 * Command line tool that refreshes the terminal/console window each second,
 * showing uptime, load average, CPU usage/stats, Memory/Swap usage, Disk
 * Activity (per drive/device), Virtual Memory activity (paging/swapping), and
 * Network traffic (per interface).
 *
 * Run this on a busy system and you can diagnose if:
 * 1) System is CPU bound
 * 2) System is RAM bound
 * 3) System is Disk bound
 * 4) System is Paging/Swapping heavily
 * 5) System is Network bound
 *
 * To exit, hit ^C.
 */
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
  DiskStats();
  ~DiskStats();

public:
  void diff(DiskStats *newer, DiskStats *older);
};

class Disk {
  std::map<std::string, DiskStats *> last, current, delta;

public:
  bool condensed;
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
  // print stats, or just return height if test is true
  uint16_t print(bool test);
};

extern Disk disk;

#endif
