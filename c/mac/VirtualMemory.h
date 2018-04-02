//
// Created by Michael Schwartz on 4/2/18.
//

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
