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
  std::map<std::string, CPU *> last, current, delta;
  Processor();
  void read(std::map<std::string, CPU *> &m);
  void copy(std::map<std::string, CPU *> &dst,
            std::map<std::string, CPU *> &src);
  void update();
  void print();
};

extern Processor processor;

#endif
