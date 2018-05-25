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
