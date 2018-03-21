#include "systat.h"

int main(int ac, char *av[]) {
  printf("Hello, world\n");
  CPUSAMPLE s1, s2, result;
  sample(&s1);
  while (1) {
    sleep(1);
    sample(&s2);
    deltaSample(s1, s2, result);
    s1 = s2;
    result.print();
    printNetwork();
  }
  return 1;
}
