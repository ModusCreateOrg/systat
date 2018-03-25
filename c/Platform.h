//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_PLATFORM_H
#define C_PLATFORM_H


class Platform {
public:
  int  cpu_count;
  int num_processes;
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
  void print();
};

extern Platform platform;

#endif //C_PLATFORM_H
