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
 */

#ifndef SYSTAT_VIRTUALMEMORY_H
#define SYSTAT_VIRTUALMEMORY_H

#include "systat.h"

struct VMStats {
  uint64_t pageins, pageouts, swapins, swapouts;
  void diff(VMStats *newer, VMStats *older);
};

class VirtualMemory {
  VMStats last, current, delta;
public:
  VirtualMemory();
public:
  void read(VMStats *stats);
  void copy(VMStats *dst, VMStats *src);
  void update();
  uint16_t print(bool test);
};

extern VirtualMemory virtual_memory;

#endif //SYSTAT_VIRTUALMEMORY_H
