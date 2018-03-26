#ifndef MEMORY_H
#define MEMORY_H

class Memory {
private:
  unsigned long ram_total, ram_used, ram_free, ram_buffers, ram_cached,
      ram_available;
  unsigned long swap_total, swap_used, swap_free;

public:
  Memory();
  ~Memory();

public:
  void update();
  void print();
};

extern Memory memory;

#endif
