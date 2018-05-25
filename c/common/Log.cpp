/*
 * systat for MacOS and Linux
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
#include "Log.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Log logger;

static FILE *fp = nullptr;
Log::Log() {
  fp = fopen("/tmp/systat.log", "w");
  if (!fp) {
    perror("Log open");
    exit(1);
  }
  fprintf(fp, "Logging activated\n");
  fflush(fp);
}

Log::~Log() { fclose(fp); }

void Log::print(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fp, fmt, ap);
  va_end(ap);
  fflush(fp);
}

void Log::println(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fp, fmt, ap);
  fputc('\n', fp);
  va_end(ap);
  fflush(fp);
}
