#ifndef C_CPU_H
#define C_CPU_H

#include "systat.h"

struct CPU {
  int        id;
  const char *name;
  uint64_t   user, nice, system, idle;
  void diff(CPU *newer, CPU *older);
  void print();
};

class Processor {
public:
  int                          num_cores;
  bool                         condensed;
  std::map<std::string, CPU *> last, current, delta;
public:
  Processor();

public:
  // returns # of processors
  uint16_t read(std::map<std::string, CPU *> &m);

  void copy(std::map<std::string, CPU *> &dst,
            std::map<std::string, CPU *> &src);

  void update();

  uint16_t print(bool test);
};

extern Processor processor;

#endif //C_CPU_H
