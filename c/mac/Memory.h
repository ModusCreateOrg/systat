//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_MEMORY_H
#define C_MEMORY_H

struct MemoryStats {
  u_long total_pages,
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
  u_long      page_size;

public:
  Memory();

private:
  void fetch(MemoryStats *stats);

public:
  void update();

  void print();
};

extern Memory memory;

#endif //C_MEMORY_H
