#include "systat.h"

int main(int ac, char *av[]) {
  setlocale(LC_NUMERIC, "");
  while (true) {
    sleep(1);
    console.moveTo(0,0);
    platform.print_header();
    processor.update();
    processor.print();
    console.moveTo(2,0);
    network.update();
    network.print();
    memory.update();
    memory.print();
  }
  return 1;
}
