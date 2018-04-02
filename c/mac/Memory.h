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
