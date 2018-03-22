//
// Created by Michael Schwartz on 3/21/18.
//

#ifndef C_PLATFORM_H
#define C_PLATFORM_H


class Platform {
public:
  int cpu_count;
  char hostname[256];
public:
  Platform();
public:
  void print_header();
};

extern Platform platform;

#endif //C_PLATFORM_H
