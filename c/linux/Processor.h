#ifndef PROCESSOR_H
#define PROCESSOR_H

struct CPU {
  const char *name;
  unsigned long user, nice, system, idle, iowait, irq, softirq;

public:
  void diff(CPU *newer, CPU *older);
  void print();
};

struct Processor {
  int num_cores;
  std::map<std::string, CPU *> last, current, delta;

public:
  Processor();

public:
  // returns # of processors
  int read(std::map<std::string, CPU *> &m);
  void copy(std::map<std::string, CPU *> &dst,
            std::map<std::string, CPU *> &src);
  void update();
  void print();
};

extern Processor processor;

#endif
