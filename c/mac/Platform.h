//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_PLATFORM_H
#define C_PLATFORM_H


class Platform {
public:
  uint16_t refresh_time;
  uint64_t uptime, idle;
  uint64_t num_processes, cpu_count;
  double loadavg[3];
  char *hostname,
       *sysname,
       *release,
       *version,
       *machine;
  kinfo_proc *kp;
public:
  Platform();

public:
  void update();
  uint16_t print(bool test);
};

extern Platform platform;

#endif //C_PLATFORM_H
