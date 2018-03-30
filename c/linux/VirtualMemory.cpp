#include "systat.h"

VirtualMemory virtual_memory;

VirtualMemory::VirtualMemory() {
  this->read(&this->current);
  this->last.copy(&this->current);
  this->delta.diff(&this->current, &this->last);
}

VirtualMemory::~VirtualMemory() {
  //
}

void VirtualMemory::read(VMStats *stats) {
  Parser p("/proc/vmstat");
  while (p.next()) {
    const char *token = p.get_token();
    if (!strncmp(token, "pgpgin", 5)) {
      stats->page_in = p.get_long();
    }
    else if (!strncmp(token, "pgpgout", 6)) {
      stats->page_out = p.get_long();
    }
    else if (!strncmp(token, "pswpin", 6)) {
      stats->swap_in = p.get_long();
    }
    else if (!strncmp(token, "pswpout", 7)) {
      stats->swap_in = p.get_long();
    }
  }
}

void VirtualMemory::update() {
  this->last.copy(&this->current);
  this->read(&this->current);
  this->delta.diff(&this->current, &this->last);
}

uint16_t VirtualMemory::print() {
  console.inverseln("%-16s %12s %23s", "VIRTUAL MEMORY", "Current",
                    "Aggregate");
  console.inverseln("%-16s %6s %6s %12s %12s", "", "IN", "OUT", "IN", "OUT");
  console.println("%-16s %6d %6d %12d %12d", "Page", this->delta.page_in,
                  this->delta.page_out, this->current.page_in,
                  this->current.page_out);
  console.println("%-16s %6d %6d %12d %12d", "Swap", this->delta.swap_in,
                  this->delta.swap_out, this->current.swap_in,
                  this->current.swap_out);
  return 4;
}
