#include <stdio.h>

#include "hello_shared.h"

void print_hello() {
  printf("Hello, World!\n");
}

int main() {
  print_hello();
  print_hello_shared();
  return 0;
}
