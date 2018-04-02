#ifndef LOG_H
#define LOG_H

// NOTE NOTE NOTE
// You must #define ENABLE_LOGGING before including this file, if you want
// logging
class Log {
public:
  Log();
  ~Log();

public:
  void print(const char *fmt, ...);
  void println(const char *fmt, ...);
};

extern Log log;

#endif
