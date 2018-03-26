#include "systat.h"

Platform platform;

Platform::Platform() {
  refresh_time = 1;
  getloadavg(this->loadavg, 3);
  uname(&this->info);
}

void Platform::update() {
  getloadavg(this->loadavg, 3);
  uname(&this->info);
}

void Platform::print() {
  time_t now = time(0);
  struct tm *p = localtime(&now);

  char s[1000];
  strftime(s, 1000, "%c", p);
  console.inverseln("systat/%d [%s/%s %s] %s", this->refresh_time,
                this->info.nodename, this->info.sysname, this->info.release, s);
  console.mode_bold(true);
  console.print("Load Average: ");
  console.mode_clear();
  console.print("%6.2f %6.2f %6.2f", this->loadavg[0], this->loadavg[1],
                this->loadavg[2]);
  console.newline();
}
