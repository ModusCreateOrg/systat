#include "Log.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

Log log;

static FILE *fp = nullptr;
Log::Log() {
#ifdef ENABLE_LOGGING
  fp = fopen("/tmp/systat.log", "w");
  if (!fp) {
    perror("Log open");
    exit(1);
  }
  fprintf(fp, "Logging activated\n");
  fflush(fp);
#endif
}

Log::~Log() {
#ifdef ENABLE_LOGGING
  fclose(fp);
#endif
}

void Log::print(const char *fmt, ...) {
#ifdef ENABLE_LOGGING
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fp, fmt, ap);
  va_end(ap);
  fflush(fp);
#endif
}

void Log::println(const char *fmt, ...) {
#ifdef ENABLE_LOGGING
  va_list ap;

  va_start(ap, fmt);
  vfprintf(fp, fmt, ap);
  fputc('\n', fp);
  va_end(ap);
  fflush(fp);
#endif
}
