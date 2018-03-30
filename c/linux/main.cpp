#include "systat.h"
#include <signal.h>

static bool alive = true;

void sigint_handler(int) { alive = false; }
int main(int ac, char *av[]) {
  uint16_t y = 0;
  signal(SIGINT, sigint_handler);
#if DEBUG != 0
  console.clear();
#endif
  while (alive) {
    console.moveTo(0, 0);
    platform.update();
    platform.print();
    y = 4;
    console.moveTo(y, 0);
    processor.update();
    processor.print();
    y += 1 + processor.num_cores + 1;
    console.moveTo(y, 0);
    memory.update();
    memory.print();
    y += 4;
    console.moveTo(y, 0);
    disk.update();
    disk.print();
    y += 2 + disk.num_disks;
    console.moveTo(y, 0);
    virtual_memory.update();
    virtual_memory.print();
    sleep(1);
  }
  //  console.println("hello, world");
  //  console.println("goodbye");
  //  sleep(3);
  return 0;
}
