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

#ifndef C_PLATFORM_H
#define C_PLATFORM_H

class Platform {
public:
  char *hostname, *sysname, *release, *version, *machine;
  kinfo_proc *kp;
  uint64_t uptime, idle;
  double loadavg[3];
  uint64_t num_processes, cpu_count;
  uint16_t refresh_time;
  uint8_t pad[6];

public:
  Platform();

public:
  void update();
  uint16_t print(bool test);
};

extern Platform platform;

#endif // C_PLATFORM_H
