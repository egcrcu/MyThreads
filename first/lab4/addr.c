#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_initialized = 1;
int global_uninitialized;
const int global_const = 5;

void f() {
    int local;
    static int local_static;
    const int local_const = 10;
    printf("local: \n");
    printf("&local = %p\n", &local);
    printf("&local_static = %p\n", &local_static);
    printf("&local_const = %p\n", &local_const);
}

int main() {
    printf("\nglobal: \n");
    printf("&global_initialized = %p\n", &global_initialized);
    printf("&global_uninitialized = %p\n", &global_uninitialized);
    printf("&global_const = %p\n\n", &global_const);
    f();
    printf("\n");
    char pid[50];
    sprintf(pid, "cat /proc/%d/maps", getpid());
    system(pid);
    return 0;
}
