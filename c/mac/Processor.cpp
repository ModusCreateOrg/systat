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
#include "systat.h"

void CPU::diff(CPU *newer, CPU *older) {
  this->user   = newer->user - older->user;
  this->nice   = newer->nice - older->nice;
  this->system = newer->system - older->system;
  this->idle   = newer->idle - older->idle;
}

void CPU::print() {
  double total = this->user + this->system + this->nice + this->idle;

  if (total) {
    console.println("%-6s %6.1f%% %6.1f%% %6.1f%% %6.1f%%",
                    this->name, 100 * this->user / total,
                    100 * this->system / total, 100 * this->nice / total);
  } else {
    console.println("%-6s %6.1f%% %6.1f%% %6.1f%% %6.1f%%",
                    this->name, 0.0, 0.0, 0.0, 0.0);
  }
}

Processor::Processor() {
  this->condensed = false;
  this->num_cores = this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

uint16_t Processor::read(std::map<std::string, CPU *> &m) {
  processor_cpu_load_info_t cpuLoad;
  mach_msg_type_number_t    processorMsgCount;
  natural_t                 processorCount;

  /* kern_return_t err = */ host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &processorCount,
                                                (processor_info_array_t *) &cpuLoad, &processorMsgCount);
  CPU *total = m["CPU"];
  if (!total) {
    total = m["CPU"] = new CPU;
    total->name = strdup("CPU");
  }
  total->system = total->user = total->nice = total->idle = 0;
  for (natural_t i = 0; i < processorCount; i++) {
    unsigned int *cpu_ticks = &cpuLoad[i].cpu_ticks[0];
    char         name[3 + 3 + 1];
    sprintf(name, "CPU%d", i);
    CPU *cpu    = m[name];
    if (!cpu) {
      cpu = m[name] = new CPU;
      cpu->name = strdup(name);
    }
    cpu->system = cpu_ticks[CPU_STATE_SYSTEM];
    cpu->user   = cpu_ticks[CPU_STATE_USER];
    cpu->nice   = cpu_ticks[CPU_STATE_NICE];
    cpu->idle   = cpu_ticks[CPU_STATE_IDLE];

    total->system += cpu->system;
    total->user += cpu->user;
    total->nice += cpu->nice;
    total->idle += cpu->idle;
  }
  return (uint16_t) processorCount;
}

void Processor::copy(std::map<std::string, CPU *> &dst,
                     std::map<std::string, CPU *> &src) {
  for (const auto &kv : src) {
    CPU        *o    = (CPU *) kv.second;
    const char *name = o->name;

    CPU *cpu    = dst[name];
    if (!cpu) {
      cpu = dst[name] = new CPU;
      cpu->name = strdup(name);
    }
    cpu->user   = o->user;
    cpu->system = o->system;
    cpu->idle   = o->idle;
    cpu->nice   = o->nice;
  }
}

void Processor::update() {
  this->copy(this->last, this->current);
  this->read(this->current);
  for (const auto &kv : this->delta) {
    CPU *cpu = (CPU *) kv.second;
    cpu->diff(this->current[cpu->name], this->last[cpu->name]);
  }
}

uint16_t Processor::print(bool test) {
  uint16_t count = 0;
  if (!test) {
    console.inverseln("%-6s %7s %7s %7s %7s ", "CPUS", "User",
                      "System", "Nice", "Idle");
  }
  count++;

  CPU *cpu = this->delta["CPU"];
  if (!test) {
    cpu->print();
  }
  count++;
  if (this->condensed) {
    return count;
  }
  for (uint16_t i = 0; i < this->num_cores; i++) {
    char name[32];
    sprintf(name, "CPU%d", i);
    CPU *cpu = this->delta[name];
    if (!test) {
      cpu->print();
    }
    count++;
  }
  return count;
}

Processor processor;

