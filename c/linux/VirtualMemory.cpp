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
  FILE *fp = fopen("/proc/vmstat", "r");
  char buf[1024];
  while (fgets(buf, 1024, fp)) {
    buf[strlen(buf) - 1] = '\0';
    if (!strncmp(buf, "pgpgin", 5)) {
      stats->page_in = atol(&buf[6]);
    }
    else if (!strncmp(buf, "pgpgout", 6)) {
      stats->page_out = atol(&buf[7]);
    }
    else if (!strncmp(buf, "pswpin", 6)) {
      stats->swap_in = atol(&buf[7]);
    }
    else if (!strncmp(buf, "pswpout", 7)) {
      stats->swap_in = atol(&buf[8]);
    }
  }
  fclose(fp);
}

void VirtualMemory::update() {
  this->last.copy(&this->current);
  this->read(&this->current);
  this->delta.diff(&this->current, &this->last);
}

void VirtualMemory::print() {
  console.inverseln("%-16s %9s %16s", "VIRTUAL MEMORY", "Current", "Aggregate");
  console.inverseln("%-16s %4s %4s %8s %8s", "", "IN", "OUT", "IN", "OUT");
  console.println("%-16s %4d %4d %8d %8d", "Page", this->delta.page_in,
                  this->delta.page_out, this->current.page_in,
                  this->current.page_out);
  console.println("%-16s %4d %4d %8d %8d", "Swap", this->delta.swap_in,
                  this->delta.swap_out, this->current.swap_in,
                  this->current.swap_out);
}
