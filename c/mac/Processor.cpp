#include "systat.h"

#if 0
CPU::CPU(int id, uint *cpu_ticks) {
  this->id = id;
  this->current.system = this->last.system = cpu_ticks[CPU_STATE_SYSTEM];
  this->current.user = this->last.user = cpu_ticks[CPU_STATE_USER];
  this->current.nice = this->last.nice = cpu_ticks[CPU_STATE_NICE];
  this->current.idle = this->last.idle = cpu_ticks[CPU_STATE_IDLE];
}

void CPU::update(uint *cpu_ticks) {
  this->current.system = cpu_ticks[CPU_STATE_SYSTEM];
  this->current.user = cpu_ticks[CPU_STATE_USER];
  this->current.nice = cpu_ticks[CPU_STATE_NICE];
  this->current.idle = cpu_ticks[CPU_STATE_IDLE];
  this->delta.system = this->current.system - this->last.system;
  this->delta.user = this->current.user - this->last.user;
  this->delta.nice = this->current.nice - this->last.nice;
  this->delta.idle = this->current.idle - this->last.idle;
  this->last = this->current;
}

void CPUStats::print() {
  uint64_t total = this->system + this->user + this->nice + this->idle;

  double onePercent = total / 100.0f;

  console.print("system: %'5.2f ", (double) this->system / (double) onePercent);
  console.print("user:   %'5.2f ", (double) this->user / (double) onePercent);
  console.print("nice:   %'5.2f ", (double) this->nice/ (double) onePercent);
  console.println("idle: %'5.2f", (double) this->idle / (double) onePercent);
}
#endif

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

