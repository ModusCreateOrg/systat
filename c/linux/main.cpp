#include "systat.h"

int main(int ac, char *av[]) {
  while (1) {
    console.clear();
    processor.update();
    processor.print();
    sleep(1);
  }
  //  console.println("hello, world");
  //  console.println("goodbye");
  //  sleep(3);
  return 0;
}
