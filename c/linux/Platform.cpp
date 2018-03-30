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
  Parser p("/proc/uptime");
  if (p.next()) {
    this->uptime = p.get_long();
    this->idle = p.get_long();
  }
}

uint16_t Platform::print(bool test) {
  if (test) {
    return 3;
  }
  // get current time in human readable form
  time_t now = time(0);
  struct tm *p = localtime(&now);
  char s[1000];
  strftime(s, 1000, "%c", p);

  // compute uptime
  const int secs_per_day = 60 * 60 * 24, secs_per_hour = 60 * 60;
  int uptime = this->uptime;
  int days = uptime / secs_per_day;
  uptime -= days * secs_per_day;
  int hours = uptime / secs_per_hour;
  int minutes = (uptime - hours * secs_per_hour) / 60;

  char out[console.width + 1];
  sprintf(out, "systat/%d [%s/%s %s]", this->refresh_time, this->info.nodename,
          this->info.sysname, this->info.release);
  size_t fill = console.width - strlen(out) - strlen(s) - 1;
  register char *ptr = &out[strlen(out)];
  while (fill > 0) {
    *ptr++ = ' ';
    fill--;
  }
  strcat(ptr, s);
  console.inverseln(out);

  console.mode_bold(true);
  console.print("Uptime: ");
  console.mode_clear();
  console.print("%d days %d:%02d  ", days, hours, minutes);

  console.mode_bold(true);
  console.print("Load Average: ");
  console.mode_clear();
  console.print("%6.2f %6.2f %6.2f", this->loadavg[0], this->loadavg[1],
                this->loadavg[2]);

  console.newline();
  return 3;
}
