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
#include "systat.h"

uint16_t render(bool);
void fit(void);

static bool alive = true, condensed = false;

void sigint_handler(int) {
  //
  alive = false;
}

static uint16_t blank_line(uint16_t row, bool test) {
  if (condensed) {
    return 0;
  }
  if (!test) {
    console.moveTo(row, 0);
    console.clear_eol();
  }
  return 1;
}

uint16_t render(bool test) {
  uint16_t y = 0;

  console.moveTo(y, 0);
  y += platform.print(test);
  y += blank_line(y, test);
  console.moveTo(y, 0);
  y += processor.print(test);
  y += blank_line(y, test);

  console.moveTo(y, 0);
  y += memory.print(test);
  y += blank_line(y, test);

  console.moveTo(y, 0);
  y += disk.print(test);
  y += blank_line(y, test);

  console.moveTo(y, 0);
  y += virtual_memory.print(test);
  y += blank_line(y, test);

  console.moveTo(y, 0);
  y += network.print(test);
  y += blank_line(y, test);

  return y;
}

/**
 * fit();
 *
 * Condense output until it fits in the current window.
 *
 * This is done by eliminating blank lines and hiding
 * detailed per CPU core stats, etc.
 */
void fit(void) {
  condensed = false;
  processor.condensed = false;
  uint16_t h = render(true);
  if (h <= console.height) {
    return;
  }
  processor.condensed = true;
  h = render(true);
  if (h <= console.height) {
    return;
  }
  condensed = true;
}

int main(/*int ac, char *av[]*/) {
  signal(SIGINT, sigint_handler);
  setlocale(LC_NUMERIC, "");
  uint16_t last_height = 0;

  while (alive) {
    platform.update();
    processor.update();
    network.update();
    memory.update();
    disk.update();
    virtual_memory.update();
    console.show_cursor(false);
    fit();
    render(false);
    uint16_t y = render(false);
    if (console.height != last_height || y > console.height) {
      console.clear();
      last_height = console.height;
    }
    sleep(1);
  }
  return 1;
}
