//
// Created by Michael Schwartz on 3/21/18.
//

#include "systat.h"
#include <sys/utsname.h>
#include <libproc.h>

Platform::Platform() {
  int    cpuCount;
  int    mib[2U]        = {CTL_HW, HW_NCPU};
  size_t sizeOfCpuCount = sizeof(cpuCount);
  int    status         = sysctl(mib, 2U, &cpuCount, &sizeOfCpuCount, NULL, 0U);
  assert(status == KERN_SUCCESS);
  this->cpu_count = cpuCount;

  utsname buf;
  uname(&buf);

  this->hostname = strdup(buf.nodename);
  this->sysname  = strdup(buf.sysname);
  this->release  = strdup(buf.release);
  this->version  = strdup(buf.version);
  this->machine  = strdup(buf.machine);

  this->kp = nullptr;
}

void Platform::update() {
  getloadavg(this->loadavg, 3);
  size_t           length  = 0;
  static const int names[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};
  sysctl(
    (int *) names,
    (sizeof(names) / sizeof(names[0])) - 1,
    nullptr, &length,
    nullptr, 0
  );
  const char *buf = new char[length];
  sysctl((int *) names,
         (sizeof(names) / sizeof(names[0])) - 1,
         (void *) buf, &length,
         NULL, 0);

  if (this->kp) {
    delete[] this->kp;
  }
  this->kp            = (kinfo_proc *) buf;
  this->num_processes = length / sizeof(kinfo_proc);
  for (int i=0; i<this->num_processes; i++) {
    kinfo_proc *p = &this->kp[0];
    printf("%d\n", p->kp_proc.p_stat);
  }
  printf("%d\n", length/sizeof(kinfo_proc));
  // ? (int)(length/sizeof(kinfo_proc)) : (int)-1;

}

void Platform::print() {
  time_t    now = time(0);
  struct tm *p  = localtime(&now);

  char s[1000];
  strftime(s, 1000, "%c", p);

  console.bg_white();
  console.fg_black();
  console.print("systat/%d [%s] %s/%s(%s) %s", 1, this->hostname, this->sysname, this->release, this->machine, s);
  console.clear_eol();
  console.mode_clear();
  console.newline();

  console.mode_bold(true);
  console.print("Load Average: ");
  console.mode_clear();
  console.print("%5.2f %5.2f %5.2f", this->loadavg[0], this->loadavg[1], this->loadavg[2]);
  console.clear_eol();
  console.newline();

}

Platform platform;

