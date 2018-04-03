/*
 * systat for MacOS
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
 *
 * This source is based upon httpL//github.com/sklinkert/mac-iostat
 * which is based upon Apple's iostat.c
 * which is based upon BSD's iostat.c
 *
 * See copyright info in iostat.txt.
 */
#ifndef SYSTAT_DISK_H
#define SYSTAT_DISK_H

#include "systat.h"

#define IOKIT 1 /* to get io_name_t in device_types.h */

#define MAXDRIVES 16    /* most drives we will record */
#define MAXDRIVENAME 31 /* largest drive name we allow */

#include <IOKit/IOTypes.h>

struct DiskStats {
  io_registry_entry_t driver;
  char *name;
  u_int64_t blocksize;
  u_int64_t total_bytes;
  u_int64_t total_read_bytes;
  u_int64_t total_written_bytes;
  u_int64_t total_transfers;
  u_int64_t total_time;
  void diff(DiskStats *newer, DiskStats *older);
};

class Disk {
public:
  uint16_t num_devices;

protected:
  std::map<std::string, DiskStats *> last, current, delta;

public:
  int record_device(io_registry_entry_t drive);

  void remove_drivelist(void *context, io_iterator_t drivelist);

  void record_drivelist(void *context, io_iterator_t drivelist);

  int record_all_devices();

protected:
  uint16_t read(std::map<std::string, DiskStats *> &m);

  void copy(std::map<std::string, DiskStats *> &dst,
            std::map<std::string, DiskStats *> &src);

public:
  Disk();

public:
  uint16_t update();

  uint16_t print(bool test);
};

extern Disk disk;

#endif // SYSTAT_DISK_H
