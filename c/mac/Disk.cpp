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
#include "Disk.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOBSD.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOBlockStorageDriver.h>
#include <IOKit/storage/IOMedia.h>

Disk disk;

void DiskStats::diff(DiskStats *newer, DiskStats *older) {
  this->total_bytes = newer->total_bytes - older->total_bytes;
  this->total_read_bytes = newer->total_read_bytes - older->total_read_bytes;
  this->total_written_bytes =
      newer->total_written_bytes - older->total_written_bytes;
  this->total_transfers = newer->total_transfers - older->total_transfers;
  this->total_time = newer->total_time - older->total_time;
}

static mach_port_t masterPort;
static IONotificationPortRef notifyPort;
static CFRunLoopSourceRef rls;

struct drivestats {
  io_registry_entry_t driver;
  char name[MAXDRIVENAME + 1];
  u_int64_t blocksize;
  u_int64_t total_bytes;
  u_int64_t total_read_bytes;
  u_int64_t total_written_bytes;
  u_int64_t total_transfers;
  u_int64_t total_time;
};

static struct drivestats drivestat[MAXDRIVES];

/*
 * Determine whether an IORegistryEntry refers to a valid
 * I/O device, and if so, record it.
 */
int Disk::record_device(io_registry_entry_t drive) {
  io_registry_entry_t parent;
  CFDictionaryRef properties;
  CFStringRef name;
  CFNumberRef number;
  kern_return_t status;

  /* get drive's parent */
  status = IORegistryEntryGetParentEntry(drive, kIOServicePlane, &parent);
  if (status != KERN_SUCCESS)
    console.abort("device has no parent");
  if (IOObjectConformsTo(parent, "IOBlockStorageDriver")) {
    drivestat[num_devices].driver = parent;

    /* get drive properties */
    status = IORegistryEntryCreateCFProperties(
        drive, (CFMutableDictionaryRef *)&properties, kCFAllocatorDefault,
        kNilOptions);
    if (status != KERN_SUCCESS)
      console.abort("device has no properties");

    /* get name from properties */
    name = (CFStringRef)CFDictionaryGetValue(properties, CFSTR(kIOBSDNameKey));
    if (name)
      CFStringGetCString(name, drivestat[num_devices].name, MAXDRIVENAME,
                         kCFStringEncodingUTF8);
    else {
      console.abort("device does not have a BSD name");
    }

    /* get blocksize from properties */
    number = (CFNumberRef)CFDictionaryGetValue(
        properties, CFSTR(kIOMediaPreferredBlockSizeKey));
    if (number)
      CFNumberGetValue(number, kCFNumberSInt64Type,
                       &drivestat[num_devices].blocksize);
    else
      console.abort("device does not have a preferred block size");

    /* clean up, return success */
    CFRelease(properties);
    num_devices++;
    return (0);
  }

  /* failed, don't keep parent */
  IOObjectRelease(parent);
  return (1);
}

static int compare_drivestats(const void *pa, const void *pb) {
  struct drivestats *a = (struct drivestats *)pa;
  struct drivestats *b = (struct drivestats *)pb;
  return strcmp(a->name, b->name);
}

void Disk::remove_drivelist(void *context, io_iterator_t drivelist) {
  io_registry_entry_t drive;
  while ((drive = IOIteratorNext(drivelist))) {
    kern_return_t status;
    char bsdname[MAXDRIVENAME];
    CFDictionaryRef properties;
    CFStringRef name;

    /* get drive properties */
    status = IORegistryEntryCreateCFProperties(
        drive, (CFMutableDictionaryRef *)&properties, kCFAllocatorDefault,
        kNilOptions);
    if (status != KERN_SUCCESS)
      continue;

    /* get name from properties */
    name = (CFStringRef)CFDictionaryGetValue(properties, CFSTR(kIOBSDNameKey));

    if (name && CFStringGetCString(name, bsdname, MAXDRIVENAME,
                                   kCFStringEncodingUTF8)) {
      int i;
      for (i = 0; i < num_devices; ++i) {
        if (strcmp(bsdname, drivestat[i].name) == 0) {
          if (i < MAXDRIVES - 1) {
            memmove(&drivestat[i], &drivestat[i + 1],
                    sizeof(struct drivestats) * (MAXDRIVES - i));
          }
          --num_devices;
          qsort(drivestat, num_devices, sizeof(struct drivestats),
                &compare_drivestats);
          break;
        }
      }
    }
    CFRelease(properties);
    IOObjectRelease(drive);
  }
}

void Disk::record_drivelist(void *context, io_iterator_t drivelist) {
  io_registry_entry_t drive;
  while ((drive = IOIteratorNext(drivelist))) {
    if (num_devices < MAXDRIVES) {
      record_device(drive);
    }
    IOObjectRelease(drive);
  }
  qsort(drivestat, num_devices, sizeof(struct drivestats), &compare_drivestats);
}

static void xrecord_drivelist(void *context, io_iterator_t drivelist) {
  disk.record_drivelist(context, drivelist);
}

static void xremove_drivelist(void *context, io_iterator_t drivelist) {
  disk.remove_drivelist(context, drivelist);
}

int Disk::record_all_devices() {
  io_iterator_t drivelist;
  CFMutableDictionaryRef match;
  kern_return_t status;

  /*
   * Get an iterator for IOMedia objects.
   */
  match = IOServiceMatching("IOMedia");
  CFDictionaryAddValue(match, CFSTR(kIOMediaWholeKey), kCFBooleanTrue);

  CFRetain(match);
  status = IOServiceAddMatchingNotification(
      notifyPort, kIOFirstMatchNotification, match, &xrecord_drivelist, NULL,
      &drivelist);

  if (status != KERN_SUCCESS)
    console.abort("couldn't match whole IOMedia devices");

  /*
   * Scan all of the IOMedia objects, and for each
   * object that has a parent IOBlockStorageDriver, save
   * the object's name and the parent (from which we can
   * fetch statistics).
   *
   * XXX What about RAID devices?
   */

  record_drivelist(NULL, drivelist);

  status = IOServiceAddMatchingNotification(
      notifyPort, kIOTerminatedNotification, match, &xremove_drivelist, NULL,
      &drivelist);

  if (status != KERN_SUCCESS)
    console.abort("couldn't match whole IOMedia device removal");

  remove_drivelist(NULL, drivelist);

  return (0);
}

Disk::Disk() {
  /*
   * Get the I/O Kit communication handle.
   */
  IOMasterPort(bootstrap_port, &masterPort);

  notifyPort = IONotificationPortCreate(masterPort);
  rls = IONotificationPortGetRunLoopSource(notifyPort);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), rls, kCFRunLoopDefaultMode);
  record_all_devices();
  this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
}

void Disk::copy(std::map<std::string, DiskStats *> &dst,
                std::map<std::string, DiskStats *> &src) {
  for (const auto &kv : src) {
    DiskStats *o = (DiskStats *)kv.second;
    const char *name = o->name;

    DiskStats *stats = dst[name];
    if (!stats) {
      stats = dst[name] = new DiskStats;
      stats->name = strdup(name);
    }
    stats->driver = o->driver;
    stats->blocksize = o->blocksize;
    stats->total_bytes = o->total_bytes;
    stats->total_read_bytes = o->total_read_bytes;
    stats->total_written_bytes = o->total_written_bytes;
    stats->total_transfers = o->total_transfers;
    stats->total_time = o->total_time;
  }
}

uint16_t Disk::read(std::map<std::string, DiskStats *> &m) {
  CFNumberRef number;
  CFDictionaryRef properties;
  CFDictionaryRef statistics;
  u_int64_t value;
  u_int64_t total_bytes, total_transfers, total_blocks, total_time;
  u_int64_t total_read_bytes, total_written_bytes;
  u_int64_t interval_bytes, interval_read_bytes, interval_written_bytes;
  u_int64_t interval_transfers, interval_blocks;
  u_int64_t interval_time;
  long double interval_mb;
  long double blocks_per_second, ms_per_transaction;
  kern_return_t status;
  int i;

  for (i = 0; i < num_devices; i++) {
    /*
     * If the drive goes away, we may not get any properties
     * for it.  So take some defaults.
     */
    total_bytes = 0;
    total_transfers = 0;
    total_time = 0;
    total_read_bytes = 0;
    total_written_bytes = 0;

    /* get drive properties */
    status = IORegistryEntryCreateCFProperties(
        drivestat[i].driver, (CFMutableDictionaryRef *)&properties,
        kCFAllocatorDefault, kNilOptions);
    if (status != KERN_SUCCESS)
      continue;

    /* get statistics from properties */
    statistics = (CFDictionaryRef)CFDictionaryGetValue(
        properties, CFSTR(kIOBlockStorageDriverStatisticsKey));
    if (statistics) {

      /*
       * Get I/O volume.
       */
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics,
               CFSTR(kIOBlockStorageDriverStatisticsBytesReadKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_bytes += value;
        total_read_bytes += value;
      }
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics,
               CFSTR(kIOBlockStorageDriverStatisticsBytesWrittenKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_bytes += value;
        total_written_bytes += value;
      }

      /*
       * Get I/O counts.
       */
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics, CFSTR(kIOBlockStorageDriverStatisticsReadsKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_transfers += value;
      }
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics, CFSTR(kIOBlockStorageDriverStatisticsWritesKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_transfers += value;
      }

      /*
       * Get I/O time.
       */
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics,
               CFSTR(kIOBlockStorageDriverStatisticsLatentReadTimeKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_time += value;
      }
      if ((number = (CFNumberRef)CFDictionaryGetValue(
               statistics,
               CFSTR(kIOBlockStorageDriverStatisticsLatentWriteTimeKey)))) {
        CFNumberGetValue(number, kCFNumberSInt64Type, &value);
        total_time += value;
      }
    }
    CFRelease(properties);

    DiskStats *stat = m[drivestat[i].name];
    if (!stat) {
      stat = m[drivestat[i].name] = new DiskStats;
      stat->name = strdup(drivestat[i].name);
    }
    stat->blocksize = drivestat[i].blocksize;
    stat->total_bytes = total_bytes;
    stat->total_read_bytes = total_read_bytes;
    stat->total_written_bytes = total_written_bytes;
    stat->total_transfers = total_transfers;
    stat->total_time = total_time;
  }
  return num_devices;
}

uint16_t Disk::update() {
  this->copy(this->last, this->current);
  this->read(this->current);

  for (const auto &kv : this->delta) {
    DiskStats *stats = (DiskStats *)kv.second;
    stats->diff(this->current[stats->name], this->last[stats->name]);
  }
  return num_devices;
}

uint16_t Disk::print(bool test) {
  uint16_t count = 0;
  if (!test) {
    console.inverseln("%-16s %13s %13s %13s %13s %13s", "DISK ACTIVITY",
                      "Block Size", "Total Bytes", "Read Bytes", "Write Bytes",
                      "Transfers");
  }
  count++;
  for (const auto &kv : this->delta) {
    if (!test) {
      DiskStats *stats = (DiskStats *)kv.second;
      console.println("%-16s %'13ld %'13ld %'13ld %'13ld %'13ld", stats->name,
                      stats->blocksize, stats->total_bytes,
                      stats->total_read_bytes, stats->total_written_bytes,
                      stats->total_transfers);
    }
    count++;
  }
  return count;
}
