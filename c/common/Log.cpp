#include "Log.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Log log;

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
