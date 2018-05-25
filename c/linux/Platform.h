/*
 * systat for Linux
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

#ifndef PLATFORM_H
#define PLATFORM_H

//
class Platform {
public:
  uint16_t refresh_time;
  double loadavg[3];
  utsname info;
  uint64_t uptime, idle;

public:
  Platform();

public:
  void sleep();
  void update();
  uint16_t print(bool test);
};

extern Platform platform;

#endif
