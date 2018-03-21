#include "systat.h"

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

  console.print("system: %5.2f ", (double) this->system / (double) onePercent);
  console.print("user:   %5.2f ", (double) this->user / (double) onePercent);
  console.print("nice:   %5.2f ", (double) this->nice/ (double) onePercent);
  console.println("idle: %5.2f", (double) this->idle / (double) onePercent);
}

Processor::Processor() {
  processor_cpu_load_info_t cpuLoad;
  mach_msg_type_number_t processorMsgCount;
  natural_t processorCount;

  kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &processorCount, (processor_info_array_t *)&cpuLoad, &processorMsgCount);
  this->num_cores = processorCount;
  for (natural_t i = 0; i < processorCount; i++) {
    this->cpus[i] = new CPU(int(i), &cpuLoad[i].cpu_ticks[0]);
  }
}

void Processor::update() {
  processor_cpu_load_info_t cpuLoad;
  mach_msg_type_number_t processorMsgCount;
  natural_t processorCount;

  kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &processorCount, (processor_info_array_t *)&cpuLoad, &processorMsgCount);
  this->num_cores = processorCount;
  for (natural_t i = 0; i < processorCount; i++) {
    unsigned int *cpu_ticks = &cpuLoad[i].cpu_ticks[0];
    this->cpus[i]->update(cpu_ticks);
    this->current.system = cpu_ticks[CPU_STATE_SYSTEM];
    this->current.user = cpu_ticks[CPU_STATE_USER];
    this->current.nice = cpu_ticks[CPU_STATE_NICE];
    this->current.idle = cpu_ticks[CPU_STATE_IDLE];
    this->delta.system = this->current.system - this->last.system;
    this->delta.user = this->current.user - this->last.user;
    this->delta.nice = this->current.nice - this->last.nice;
    this->delta.idle = this->current.idle - this->last.idle;
  }
  this->last = this->current;
}

void Processor::print() {
  this->delta.print();
  console.newline();
}

Processor processor;

