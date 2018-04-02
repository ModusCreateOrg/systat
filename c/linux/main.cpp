#include "systat.h"
#include <signal.h>

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

int render(bool test) {
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
 * fill();
 *
 * Condense output until it fits in the current window.
 *
 * This is done by eliminating blank lines and hiding
 * detailed per CPU core stats, etc.
 */
void fit() {
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

int main(int ac, char *av[]) {
  logger.println("Systat Main");
  signal(SIGINT, sigint_handler);
#if DEBUG != 0
  console.clear();
#endif
  uint16_t last_height = 0;
  while (alive) {
    platform.update();
    processor.update();
    memory.update();
    disk.update();
    virtual_memory.update();
    network.update();
    console.show_cursor(false);
    fit();
    uint16_t y = render(false);
    if (console.height != last_height || y > console.height) {
      console.clear();
      //      if (y > console.height) {
      //        if (!processor.condensed) {
      //          logger.println("condensed");
      //          processor.condensed = true;
      //        }
      //        else {
      //          condensed = true;
      //        }
      //      }
      //      else {
      //        logger.println("fits");
      //        condensed = false;
      //        processor.condensed = false;
      //      }
      last_height = console.height;
      //      logger.println("main %d %d %s", y, console.height,
      //                  condensed ? "true" : "false");
    }
    sleep(1);
  }
  return 0;
}
