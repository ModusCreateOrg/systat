//
// Created by Michael Schwartz on 3/21/18.
//

#include "systat.h"

Memory::Memory() {
  this->page_size = sysconf(_SC_PAGESIZE);

  fetch(&this->last);
  this->update();
}

void Memory::fetch(MemoryStats *stats) {
  mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
  vm_statistics64_data_t vmstat;
  host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t) &vmstat, &count);
  stats->free_count     = vmstat.free_count;
  stats->active_count   = vmstat.active_count;
  stats->inactive_count = vmstat.inactive_count;
  stats->wire_count     = vmstat.wire_count;
  stats->pageins        = vmstat.pageins;
  stats->pageouts       = vmstat.pageouts;
  stats->faults         = vmstat.faults;
  stats->swapins        = vmstat.swapins;
  stats->swapouts       = vmstat.swapouts;

  int vmmib[2] = {CTL_VM, VM_SWAPUSAGE};
  xsw_usage swapused; /* defined in sysctl.h */
  size_t swlen = sizeof(swapused);
  sysctl(vmmib, 2, &swapused, &swlen, NULL, 0);

  stats->total_pages = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count + vmstat.compressor_page_count;
  stats->memory_size = stats->total_pages * page_size;
  stats->memory_used = this->page_size * (vmstat.wire_count + vmstat.active_count + vmstat.compressor_page_count + vmstat.inactive_count);
  stats->memory_free = stats->memory_size - stats->memory_used - vmstat.inactive_count;

  stats->swap_size = swapused.xsu_total;
  stats->swap_used = swapused.xsu_used;
  stats->swap_free = swapused.xsu_avail;
//  load->swapUsed = swapused.xsu_used ;
//  load->swapSize = swapused.xsu_total;
}

void Memory::update() {
  MemoryStats *current = &this->current,
              *last    = &this->last,
              *delta   = &this->delta;

  this->fetch(&this->current);

  delta->free_count     = current->free_count - last->free_count;
  delta->active_count   = current->active_count - last->active_count;
  delta->inactive_count = current->inactive_count - last->inactive_count;
  delta->wire_count     = current->wire_count - last->wire_count;
  delta->pageins        = current->pageins - last->pageins;
  delta->pageouts       = current->pageouts - last->pageouts;
  delta->faults         = current->faults - last->faults;
  delta->swapins        = current->swapins - last->swapins;
  delta->swapouts       = current->swapouts - last->swapouts;

  delta->swap_size       = current->swap_size - last->swap_size;
  delta->swap_used       = current->swap_used - last->swap_used;
  delta->swap_free       = current->swap_free - last->swap_free;
}

void Memory::print() {
  console.bg_white();
  console.fg_black();
  console.print("%-12s %9s %9s %9s %9s", "MEMORY", "Total", "Used", "Free", "Wired");
  console.clear_eol();
  console.mode_clear();
  console.newline();
  console.print("%-12s %'9lld %'9lld %'9lld %'9lld\n", "Real", this->current.memory_size/1024/1024, this->current.memory_used/1024/1024, this->current.memory_free/1024/1024, this->current.wire_count);
  console.print("%-12s %'9lld %'9lld %'9lld\n", "Swap", this->current.swap_size/1024/1024, this->current.swap_used/1024/1024, this->current.swap_free/1024/1024);
  console.newline();
  console.bg_white();
  console.fg_black();
  console.print("%-16s %19s %19s", "VIRTUAL MEMORY", "Current   ", "Aggregate  ");
  console.clear_eol();
  console.newline();
  console.print("%-16s %9s %9s %9s %9s", "", "IN", "OUT", "IN", "OUT");
  console.clear_eol();
  console.mode_clear();
  console.newline();
  console.print("%-16s %'9lld %'9lld %'9lld %'9lld\n", "Page", this->delta.pageins/1024/1024, this->delta.pageouts/1024/1024, this->current.pageins/1024/1024, this->current.pageouts/10924/1024);
  console.print("%-16s %'9lld %'9lld %'9lld %'9lld\n", "Swap", this->delta.swapins/1024/1024, this->delta.swapouts/1024/1024, this->current.swapins/1024/1024, this->current.swapouts/10924/1024);
}

Memory memory;