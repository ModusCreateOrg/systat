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
  void print();
};

extern Platform platform;

#endif
