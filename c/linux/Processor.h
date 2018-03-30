#ifndef PROCESSOR_H
#define PROCESSOR_H

struct CPU {
  const char *name;
  uint64_t  user, nice, system, idle, iowait, irq, softirq;

public:
  void diff(CPU *newer, CPU *older);
  void print();
};

struct Processor {
  uint16_t num_cores;
  std::map<std::string, CPU *> last, current, delta;

public:
  Processor();

public:
  // returns # of processors
  uint16_t read(std::map<std::string, CPU *> &m);
  void copy(std::map<std::string, CPU *> &dst,
            std::map<std::string, CPU *> &src);
  void update();
  void print();
};

extern Processor processor;

#endif
