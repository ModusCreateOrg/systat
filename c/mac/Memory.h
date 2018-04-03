/*
 * This source is based upon httpL//github.com/sklinkert/mac-iostat
 * which is based upon Apple's iostat.c
 * which is based upon BSD's iostat.c
 *
 * See copyright info in iostat.txt.
 */

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
#ifndef C_MEMORY_H
#define C_MEMORY_H

struct MemoryStats {
  uint64_t total_pages,
           memory_used,
           memory_size,
           memory_free,
           free_count,
           active_count,
           inactive_count,
           wire_count,
           pageins,
           pageouts,
           faults,
           swapins,
           swapouts,
           swap_used,
           swap_free,
           swap_size;
};

class Memory {
public:
  MemoryStats last, current, delta;
  uint64_t    page_size;

public:
  Memory();

private:
  void read(MemoryStats *stats);

public:
  void update();

  // print memory stats unless test is set
  uint16_t print(bool test);
};

extern Memory memory;

#endif //C_MEMORY_H
