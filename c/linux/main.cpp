#include "systat.h"

int main(int ac, char *av[]) {
  console.clear();
  while (1) {
    console.moveTo(0, 0);
    platform.update();
    platform.print();
    console.moveTo(4, 0);
    processor.update();
    processor.print();
    console.moveTo(4 + 2 + processor.num_cores + 1, 0);
    memory.update();
    memory.print();
    sleep(1);
  }
  //  console.println("hello, world");
  //  console.println("goodbye");
  //  sleep(3);
  return 0;
}
