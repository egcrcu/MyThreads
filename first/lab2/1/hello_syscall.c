#include <unistd.h>
#include <syscall.h>

void write_syscall(int fd, void* buffer, size_t length) {
  syscall(SYS_write, 1, buffer, length);
}

int main() {
  write_syscall(1, "Hello, World! from syscall()", 28);
  return 0;
}
