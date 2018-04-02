//
// Created by Michael Schwartz on 3/21/18.
//

#include "systat.h"
#include <sys/utsname.h>
#include <libproc.h>

static uint64_t get_uptime() {
  timeval boottime;
  size_t  len    = sizeof(boottime);
  int     mib[2] = {CTL_KERN, KERN_BOOTTIME};
  if (sysctl(mib, 2, &boottime, &len, nullptr, 0) < 0) {
    return static_cast<uint64_t>(-1);
  }
  time_t bsec = boottime.tv_sec, csec = time(nullptr);

  return static_cast<uint64_t>(difftime(csec, bsec));
}

Platform::Platform() {
  this->refresh_time    = 1;
  int    cpuCount;
  int    mib[2U]        = {CTL_HW, HW_NCPU};
  size_t sizeOfCpuCount = sizeof(cpuCount);
  int    status         = sysctl(mib, 2U, &cpuCount, &sizeOfCpuCount, nullptr, 0U);
  assert(status == KERN_SUCCESS);
  this->cpu_count = static_cast<uint64_t>(cpuCount);

  utsname buf{};
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
         nullptr, 0);


  delete[] this->kp;

  this->kp            = (kinfo_proc *) buf;
  this->num_processes = length / sizeof(kinfo_proc);
  for (int i = 0; i < this->num_processes; i++) {
    kinfo_proc *p = &this->kp[0];
  }
}

uint16_t Platform::print(bool test) {
  this->uptime = get_uptime();
  if (test) {
    return 3;
  }
  time_t    now = time(0);
  struct tm *p  = localtime(&now);

  char s[1000];
  strftime(s, 1000, "%c", p);

  // compute uptime
  const int secs_per_day = 60 * 60 * 24, secs_per_hour = 60 * 60;
  int       uptime       = this->uptime;
  int       days         = uptime / secs_per_day;
  uptime -= days * secs_per_day;
  int hours   = uptime / secs_per_hour;
  int minutes = (uptime - hours * secs_per_hour) / 60;

  const int width = console.width ? console.width : 80;
  char out[width + 1];
  sprintf(out, "systat/%d [%s/%s %s]", this->refresh_time, this->hostname, this->sysname, this->release);
  size_t        fill = width - strlen(out) - strlen(s) - 1;
  char *ptr = &out[strlen(out)];
  while (fill > 0) {
    *ptr++ = ' ';
    fill--;
  }
  strcat(ptr, s);
  console.inverseln(out);
//  console.inverseln("systat/%d [%s] %s/%s(%s) %s", 1, this->hostname, this->sysname, this->release, this->machine, s);

  console.mode_bold(true);
  console.print("Uptime: ");
  console.mode_clear();
  console.print("%d days %d:%02d  ", days, hours, minutes);
  console.mode_bold(true);
  console.print("Load Average: ");
  console.mode_clear();
  console.print("%5.2f %5.2f %5.2f", this->loadavg[0], this->loadavg[1], this->loadavg[2]);
  console.clear_eol();
  console.newline();
  return 3;
}

Platform platform;

