#include "systat.h"

void sample(CPUSAMPLE *sample) {
  processor_cpu_load_info_t cpuLoad;
  mach_msg_type_number_t processorMsgCount;
  natural_t processorCount;

  uint64_t totalSystemTime = 0, totalUserTime = 0, totalIdleTime = 0;

  kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &processorCount, (processor_info_array_t *)&cpuLoad, &processorMsgCount);

  for (natural_t i = 0; i < processorCount; i++) {

    // Calc load types and totals, with guards against 32-bit overflow
    // (values are natural_t)
    uint64_t system = 0, user = 0, idle = 0;

    system = cpuLoad[i].cpu_ticks[CPU_STATE_SYSTEM];
    user = cpuLoad[i].cpu_ticks[CPU_STATE_USER] + cpuLoad[i].cpu_ticks[CPU_STATE_NICE];
    idle = cpuLoad[i].cpu_ticks[CPU_STATE_IDLE];

    totalSystemTime += system;
    totalUserTime += user;
    totalIdleTime += idle;
  }
  sample->totalSystemTime = totalSystemTime;
  sample->totalUserTime = totalUserTime;
  sample->totalIdleTime = totalIdleTime; 
}

void deltaSample(CPUSAMPLE &s1, CPUSAMPLE &s2, CPUSAMPLE &result) {
  result.totalSystemTime = s2.totalSystemTime - s1.totalSystemTime;
  result.totalUserTime = s2.totalUserTime - s1.totalUserTime;
  result.totalIdleTime = s2.totalIdleTime - s1.totalIdleTime;
}

