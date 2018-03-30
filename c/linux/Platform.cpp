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
  FILE *fp = fopen("/proc/uptime", "r");
  char *line = nullptr;
  size_t len = 0;
  getline(&line, &len, fp);
  Line l = Line(line);
  const char *token = l.get_token();
  this->uptime = atol(token);
  delete token;

  token = l.get_token();
  this->idle = atol(token);
  delete token;

  fclose(fp);
}

void Platform::print() {
  // get current time in human readable form
  time_t now = time(0);
  struct tm *p = localtime(&now);
  char s[1000];
  strftime(s, 1000, "%c", p);

  // compute uptime/idle time
  const int secs_per_day = 60 * 60 * 24, secs_per_hour = 60 * 60;
  int uptime = this->uptime;
  int days = uptime / secs_per_day;
  uptime -= days * secs_per_day;
  int hours = uptime / secs_per_hour;
  int minutes = (uptime - hours * secs_per_hour) / 60;
  double idle = double(this->idle) / double(this->uptime);
  console.inverseln("systat/%d [%s/%s %s] %s", this->refresh_time,
                    this->info.nodename, this->info.sysname, this->info.release,
                    s);
  console.mode_bold(true);
  console.print("Uptime: ");
  console.mode_clear();
  console.print("%d days %d:%02d (%d%% idle)  ", days, hours, minutes,
                int(idle * 100));
  console.mode_bold(true);
  console.print("Load Average: ");
  console.mode_clear();
  console.print("%6.2f %6.2f %6.2f", this->loadavg[0], this->loadavg[1],
                this->loadavg[2]);
  console.newline();
}
