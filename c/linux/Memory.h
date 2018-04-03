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

#ifndef MEMORY_H
#define MEMORY_H

class Memory {
private:
  uint64_t ram_total, ram_used, ram_free, ram_buffers, ram_cached,
      ram_available;
  uint64_t swap_total, swap_used, swap_free;

public:
  Memory();
  ~Memory();

public:
  void update();
  uint16_t print(bool test);
};

extern Memory memory;

#endif
