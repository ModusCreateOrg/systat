#include "systat.h"

int main(int ac, char *av[]) {
  while (true) {
    sleep(1);
    console.moveTo(0,0);
    processor.update();
    processor.print();
    console.moveTo(2,0);
    network.update();
    network.print();
  }
  return 1;
}
