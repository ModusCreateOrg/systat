/*
 * systat for MacOS
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

#include "VirtualMemory.h"

VirtualMemory virtual_memory;

void VMStats::diff(VMStats *newer, VMStats *older) {
  this->pageins = newer->pageins - older->pageins;
  this->pageouts = newer->pageouts - older->pageouts;
  this->swapins = newer->swapins - older->swapins;
  this->swapouts = newer->swapouts - older->swapouts;
}

VirtualMemory::VirtualMemory() {
  this->read(&this->current);
  this->copy(&this->last, &this->current);
  this->copy(&this->delta, &this->current);
  this->update();
}

void VirtualMemory::copy(VMStats *dst, VMStats *src) {
  dst->pageins = src->pageins;
  dst->pageouts = src->pageouts;
  dst->swapins = src->swapins;
  dst->swapouts = src->swapouts;
}

void VirtualMemory::read(VMStats *stats) {
  stats->pageins = memory.current.pageins;
  stats->pageouts = memory.current.pageouts;
  stats->swapins = memory.current.swapins;
  stats->swapouts = memory.current.swapouts;
}

void VirtualMemory::update() {
  this->copy(&this->last, &this->current);
  this->read(&this->current);
  this->delta.diff(&this->current, &this->last);
}

uint16_t VirtualMemory::print(bool test) {
  if (test) {
    return 4;
  }
  console.inverseln("%-16s %12s %23s", "VIRTUAL MEMORY", "Current",
                    "Aggregate");
  console.inverseln("%-16s %6s %6s %12s %12s", "", "IN", "OUT", "IN", "OUT");
  console.println("%-16s %6d %6d %12d %12d", "Page", this->delta.pageins,
                  this->delta.pageouts, this->current.pageins,
                  this->current.pageouts);
  console.println("%-16s %6d %6d %12d %12d", "Swap", this->delta.swapins,
                  this->delta.swapouts, this->current.swapins,
                  this->current.swapouts);
  return 4;
}
