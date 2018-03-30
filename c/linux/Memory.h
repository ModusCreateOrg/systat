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
