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
      delete token;
      token = l.get_token();
      this->ram_total = atol(token);
    }
    else if (!strcmp(token, "MemFree:")) {
      delete token;
      token = l.get_token();
      this->ram_free = atol(token);
    }
    else if (!strcmp(token, "MemAvailable:")) {
      delete token;
      token = l.get_token();
      this->ram_available = atol(token);
    }
    else if (!strcmp(token, "Buffers:")) {
      delete token;
      token = l.get_token();
      this->ram_buffers = atol(token);
    }
    else if (!strcmp(token, "Cached:")) {
      delete token;
      token = l.get_token();
      this->ram_cached = atol(token);
    }
    else if (!strcmp(token, "SwapTotal:")) {
      delete token;
      token = l.get_token();
      this->swap_total = atol(token);
    }
    else if (!strcmp(token, "SwapFree:")) {
      delete token;
      token = l.get_token();
      this->swap_free = atol(token);
    }
    delete token;
    line = nullptr;
    len = 0;
  }
  fclose(fp);
  this->ram_used = this->ram_total - this->ram_free;
  this->ram_available = this->ram_free + this->ram_buffers + this->ram_cached;
  this->swap_used = this->swap_total - this->swap_free;
}

void Memory::print() {
  console.inverseln("%-16s %8s %8s %8s %8s %8s %8s", "MEMORY (KB)", "Total",
                    "Used", "Free", "Buffer", "Cache", "Avail");
  console.println("%-16s %8d %8d %8d %8d %8d %8d", "Real", this->ram_total,
                  this->ram_used, this->ram_free, this->ram_buffers,
                  this->ram_cached, this->ram_available);
  console.println("%-16s %8d %8d %8d", "Swap", this->swap_total,
                  this->swap_used, this->swap_free);
}
