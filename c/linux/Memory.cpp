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
  FILE *fp = fopen("/proc/meminfo", "r");
  char *line = nullptr;
  size_t len = 0;
  while (getline(&line, &len, fp) >= 0) {
    Line l = Line(line);
    const char *token = l.get_token();
    if (!strcmp(token, "MemTotal:")) {
      this->ram_total = atol(l.get_token());
    }
    else if (!strcmp(token, "MemFree:")) {
      this->ram_free = atol(l.get_token());
    }
    else if (!strcmp(token, "MemAvailable:")) {
      this->ram_available = atol(l.get_token());
    }
    else if (!strcmp(token, "Buffers:")) {
      this->ram_buffers = atol(l.get_token());
    }
    else if (!strcmp(token, "Cached:")) {
      this->ram_cached = atol(l.get_token());
    }
    else if (!strcmp(token, "SwapTotal:")) {
      this->swap_total = atol(l.get_token());
    }
    else if (!strcmp(token, "SwapFree:")) {
      this->swap_total = atol(l.get_token());
    }
    delete[] token;
    line = nullptr;
    len = 0;
  }
  fclose(fp);
  this->ram_used = this->ram_total - this->ram_free;
  this->ram_available = this->ram_free + this->ram_buffers + this->ram_cached;
  this->swap_used = this->swap_total - this->swap_free;
}

void Memory::print() {
  console.inverseln("%-16s %8s %8s %8s %8s %8s %8s", "MEMORY", "Total", "Used",
                    "Free", "Buffer", "Cache", "Avail");
  console.println("%-16s %8d %8d %8d %8d %8d %8d", "Real", this->ram_total,
                  this->ram_used, this->ram_free, this->ram_buffers,
                  this->ram_cached, this->ram_available);
  console.println("%-16s %8d %8d %8d", "Swap", this->swap_total,
                  this->swap_used, this->swap_free);
}
