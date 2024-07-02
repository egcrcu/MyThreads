#include <stdio.h>

#include "hello_static.h"

void print_hello() {
  printf("Hello, World!\n");
}

int main() {
  print_hello();
  print_hello_static();
  return 0;
}
