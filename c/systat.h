#ifndef SYSTAT_H
#define SYSTAT_H

// global header file for systat program

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_mib.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <net/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/types.h>

#include <errno.h>
#include <mach/mach_host.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <unistd.h>

#include <ncurses.h>

struct CPUSAMPLE {
  uint64_t totalSystemTime;
  uint64_t totalUserTime;
  uint64_t totalIdleTime;

  void print() {
    uint64_t total =
        this->totalSystemTime + this->totalUserTime + this->totalIdleTime;

    double onePercent = total / 100.0f;

    printf("system: %5.2f\n",
           (double)this->totalSystemTime / (double)onePercent);
    printf("user:   %5.2f\n", (double)this->totalUserTime / (double)onePercent);
    printf("idle:   %5.2f\n", (double)this->totalIdleTime / (double)onePercent);
    printf("\n");
    fflush(stdout);
  }
};

extern void sample(CPUSAMPLE *sample);
extern void deltaSample(CPUSAMPLE &s1, CPUSAMPLE &s2, CPUSAMPLE &result);
extern void printNetwork();

#endif
