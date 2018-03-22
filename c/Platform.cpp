//
// Created by Michael Schwartz on 3/21/18.
//

#include "systat.h"

Platform::Platform() {
  int cpuCount;
  int mib[2U] = { CTL_HW, HW_NCPU };
  size_t sizeOfCpuCount = sizeof(cpuCount);
  int status = sysctl(mib, 2U, &cpuCount, &sizeOfCpuCount, NULL, 0U);
  assert(status == KERN_SUCCESS);
  this->cpu_count = cpuCount;

  gethostname(this->hostname, 256);
}

void Platform::print_header() {
  console.bg_white();
  console.fg_black();
  console.print("systat/%d [%s]", 1, this->hostname);
  console.clear_eol();
  console.mode_clear();
  console.newline();
}

Platform platform;

