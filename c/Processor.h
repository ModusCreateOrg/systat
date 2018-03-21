//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_CPU_H
#define C_CPU_H

#include "systat.h"

struct CPUSAMPLE {
  uint64_t totalSystemTime;
  uint64_t totalUserTime;
  uint64_t totalNiceTime;
  uint64_t totalIdleTime;

  void print() {
    uint64_t total = this->totalSystemTime + this->totalUserTime + this->totalIdleTime;

    double onePercent = total / 100.0f;

    console.print("system: %5.2f ", (double) this->totalSystemTime / (double) onePercent);
    console.print("user:   %5.2f ", (double) this->totalUserTime / (double) onePercent);
    console.print("nice:   %5.2f ", (double) this->totalNiceTime / (double) onePercent);
    console.print("idle: %5.2f", (double) this->totalIdleTime / (double) onePercent);
  }
};

struct CPUStats {
  uint64_t system,
    user,
    nice,
    idle;
public:
  void print();
};

struct CPU {
  int id;
  CPUStats last,
    current,
    delta;
public:
  CPU(int id, uint *cpu_ticks);
  void update(uint *cpu_ticks);
};

class Processor {
public:
  CPU *cpus[512];
  int num_cores;
  CPUStats last, current, delta;
public:
  Processor();

public:
  void update();

  void print();
};

//extern void sample(CPUSAMPLE *sample);
//
//extern void deltaSample(CPUSAMPLE &s1, CPUSAMPLE &s2, CPUSAMPLE &result);

extern Processor processor;

#endif //C_CPU_H
