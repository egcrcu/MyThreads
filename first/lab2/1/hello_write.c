#include <unistd.h>

int main() {
  write(1, "Hello, World! from write()", 26);
  return 0;
}
