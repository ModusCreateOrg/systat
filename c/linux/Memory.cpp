/*
 * systat for Linux
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

#include "systat.h"

Memory memory;

Memory::Memory() {
  //
  this->ram_total = this->ram_used = this->ram_free = this->ram_buffers =
      this->ram_cached = this->ram_available = 0;
  this->swap_total = this->swap_used = this->swap_free = 0;
}

Memory::~Memory() {
  //
}

void Memory::update() {
  //
  Parser p("/proc/meminfo");
  while (p.next()) {
    const char *token = p.get_token();
    if (!strcmp(token, "MemTotal:")) {
      this->ram_total = p.get_long();
    }
    else if (!strcmp(token, "MemFree:")) {
      this->ram_free = p.get_long();
    }
    else if (!strcmp(token, "MemAvailable:")) {
      this->ram_available = p.get_long();
    }
    else if (!strcmp(token, "Buffers:")) {
      this->ram_buffers = p.get_long();
    }
    else if (!strcmp(token, "Cached:")) {
      this->ram_cached = p.get_long();
    }
    else if (!strcmp(token, "SwapTotal:")) {
      this->swap_total = p.get_long();
    }
    else if (!strcmp(token, "SwapFree:")) {
      this->swap_free = p.get_long();
    }
  }
  this->ram_used = this->ram_total - this->ram_free;
  this->ram_available = this->ram_free + this->ram_buffers + this->ram_cached;
  this->swap_used = this->swap_total - this->swap_free;
}

uint16_t Memory::print(bool test) {
  if (!test) {
    console.inverseln("%-16s %8s %8s %8s %8s %8s %8s", "MEMORY (KB)", "Total",
                      "Used", "Free", "Buffer", "Cache", "Avail");
    console.println("%-16s %8d %8d %8d %8d %8d %8d", "Real", this->ram_total,
                    this->ram_used, this->ram_free, this->ram_buffers,
                    this->ram_cached, this->ram_available);
    console.println("%-16s %8d %8d %8d", "Swap", this->swap_total,
                    this->swap_used, this->swap_free);
  }
  return 3;
}
