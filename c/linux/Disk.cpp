#include "systat.h"
#include <dirent.h>

Disk disk;

void DiskStats::diff(DiskStats *newer, DiskStats *older) {
  this->reads_issued = newer->reads_issued - older->reads_issued;
  this->reads_merged = newer->reads_merged - older->reads_merged;
  this->sectors_read = newer->sectors_read - older->sectors_read;
  this->time_reading = newer->time_reading - older->time_reading;
  this->writes_completed = newer->writes_completed - older->writes_completed;
  this->writes_merged = newer->writes_merged - older->writes_merged;
  this->sectors_written = newer->sectors_written - older->sectors_written;
  this->time_writing = newer->time_writing - older->time_writing;
  this->ios_in_progress = newer->ios_in_progress - older->ios_in_progress;
  this->time_doing_ios = newer->time_doing_ios - older->time_doing_ios;
  this->weighted_time_ios = newer->weighted_time_ios - older->weighted_time_ios;
}

Disk::Disk() {
  this->num_disks = this->read(this->current);
  this->copy(this->last, this->current);
  this->copy(this->delta, this->current);
  this->update();
}

Disk::~Disk() {
  //
}

void Disk::copy(std::map<std::string, DiskStats *> &dst,
                std::map<std::string, DiskStats *> &src) {
  for (const auto &kv : src) {
    DiskStats *o = (DiskStats *)kv.second;
    const char *name = o->name;

    DiskStats *stats = dst[name];
    if (!stats) {
      stats = dst[name] = new DiskStats;
      stats->name = strdup(name);
    }
    stats->reads_issued = o->reads_issued;
    stats->reads_merged = o->reads_merged;
    stats->sectors_read = o->sectors_read;
    stats->time_reading = o->time_reading;
    stats->writes_completed = o->writes_completed;
    stats->writes_merged = o->writes_merged;
    stats->sectors_written = o->sectors_written;
    stats->time_writing = o->time_writing;
    stats->ios_in_progress = o->ios_in_progress;
    stats->time_doing_ios = o->time_doing_ios;
    stats->weighted_time_ios = o->weighted_time_ios;
  }
}

uint16_t Disk::read(std::map<std::string, DiskStats *> &s) {
  uint16_t count = 0;
  FILE *fp = fopen("/proc/diskstats", "r");
  char *line = nullptr;
  size_t len = 0;
  while (getline(&line, &len, fp) > 0) {
    Line l = Line(line);
    const char *token = l.get_token();
    uint64_t major = atol(token);
    delete[] token;

    token = l.get_token();
    uint64_t minor = atol(token);
    delete[] token;

    if (minor == 0) {
      token = l.get_token();
      if (!strncmp(token, "loop", 4) || !strncmp(token, "dm-", 3)) {
        delete[] token;
        line = nullptr;
        len = 0;
        continue;
      }

      count++;
      DiskStats *info = s[token];
      if (!info) {
        info = s[token] = new DiskStats;
        info->name = token;
      }

      token = l.get_token();
      info->reads_issued = atol(token);
      delete[] token;

      token = l.get_token();
      info->reads_merged = atol(token);
      delete[] token;

      token = l.get_token();
      info->sectors_read = atol(token);
      delete[] token;

      token = l.get_token();
      info->time_reading = atol(token);
      delete[] token;

      token = l.get_token();
      info->writes_completed = atol(token);
      delete[] token;

      token = l.get_token();
      info->writes_merged = atol(token);
      delete[] token;

      token = l.get_token();
      info->sectors_written = atol(token);
      delete[] token;

      token = l.get_token();
      info->time_writing = atol(token);
      delete[] token;

      token = l.get_token();
      info->ios_in_progress = atol(token);
      delete[] token;

      token = l.get_token();
      info->time_doing_ios = atol(token);
      delete[] token;

      token = l.get_token();
      info->weighted_time_ios = atol(token);
      delete[] token;
    }
    line = nullptr;
    len = 0;
  }
  fclose(fp);
  return count;
}

void Disk::update() {
  this->copy(this->last, this->current);
  this->read(this->current);
  for (const auto &kv : this->delta) {
    DiskStats *diskstats = (DiskStats *)kv.second;
    diskstats->diff(this->current[diskstats->name],
                    this->last[diskstats->name]);
  }
}

void Disk::print() {
  console.inverseln("%-16s %5s %5s", "DISK ACTIVITY", "IN", "OUT");
  for (const auto &kv : this->delta) {
    DiskStats *info = (DiskStats *)kv.second;
    console.println("%-16s %5d %5d", info->name, info->sectors_read,
                    info->sectors_written);
  }
}
