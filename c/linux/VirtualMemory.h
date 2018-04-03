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

#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

struct VMStats {
  uint64_t page_in, page_out;
  uint64_t swap_in, swap_out;
  void diff(VMStats *newer, VMStats *older) {
    this->page_in = newer->page_in - older->page_in;
    this->page_out = newer->page_out - older->page_out;
    this->swap_in = newer->swap_in - older->swap_in;
    this->swap_out = newer->swap_out - older->swap_out;
  }
  void copy(VMStats *other) {
    this->page_in = other->page_in;
    this->page_out = other->page_out;
    this->swap_in = other->swap_in;
    this->swap_out = other->swap_out;
  }
};

class VirtualMemory {
private:
  VMStats last, current, delta;

public:
  VirtualMemory();
  ~VirtualMemory();

private:
  void read(VMStats *stats);

public:
  void update();
  uint16_t print(bool test);
};

extern VirtualMemory virtual_memory;

#endif
