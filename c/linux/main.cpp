#include "systat.h"
#include <signal.h>

static bool alive = true, condensed = false;

void sigint_handler(int) {
  //
  alive = false;
}

static uint16_t blank_line(uint16_t row) {
  if (condensed) {
    return 0;
  }
  console.moveTo(row, 0);
  console.clear_eol();
  return 1;
}

int main(int ac, char *av[]) {
  log.println("Systat Main");
  signal(SIGINT, sigint_handler);
#if DEBUG != 0
  console.clear();
#endif
  uint16_t last_height = 0;
  while (alive) {
    console.show_cursor(false);
    uint16_t y = 0;
    console.moveTo(0, 0);
    platform.update();
    y += platform.print();
    y += blank_line(y);

    console.moveTo(y, 0);
    processor.update();
    y += processor.print();
    y += blank_line(y);

    console.moveTo(y, 0);
    memory.update();
    y += memory.print();
    y += blank_line(y);

    console.moveTo(y, 0);
    disk.update();
    y += disk.print();
    y += blank_line(y);

    console.moveTo(y, 0);
    virtual_memory.update();
    y += virtual_memory.print();
    y += blank_line(y);

    console.moveTo(y, 0);
    network.update();
    y += network.print();
    y += blank_line(y);

    if (console.height != last_height || y > console.height) {
      console.clear();
      if (y > console.height) {
        if (!processor.condensed) {
          log.println("condensed");
          processor.condensed = true;
        }
        else {
          condensed = true;
        }
      }
      else {
        log.println("fits");
        condensed = false;
        processor.condensed = false;
      }
      last_height = console.height;
      log.println("main %d %d %s", y, console.height,
                  condensed ? "true" : "false");
    }
    sleep(1);
  }
  return 0;
}
