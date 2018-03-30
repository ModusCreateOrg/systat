#ifndef LOG_H
#define LOG_H

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
