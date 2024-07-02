#include <stdio.h>

int *create_local() {
    int local = 10;
    printf("&local = %p\n", &local);
    return &local;
}

int main() {
    int *ptr = create_local();
    printf("*ptr = %d\n", *ptr);
    return 0;
}

