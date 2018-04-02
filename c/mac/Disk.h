//
// Created by Michael Schwartz on 4/2/18.
//

#ifndef SYSTAT_DISK_H
#define SYSTAT_DISK_H

#include "systat.h"

#define IOKIT  1  /* to get io_name_t in device_types.h */

#define MAXDRIVES  16  /* most drives we will record */
#define MAXDRIVENAME  31  /* largest drive name we allow */

#include <IOKit/IOTypes.h>

struct DiskStats {
  io_registry_entry_t driver;
  char                *name;
  u_int64_t           blocksize;
  u_int64_t           total_bytes;
  u_int64_t           total_read_bytes;
  u_int64_t           total_written_bytes;
  u_int64_t           total_transfers;
  u_int64_t           total_time;
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

  void copy(std::map<std::string, DiskStats *> &dst, std::map<std::string, DiskStats *> &src);

public:
  Disk();

public:
  uint16_t update();

  uint16_t print(bool test);

};

extern Disk disk;


#endif //SYSTAT_DISK_H
