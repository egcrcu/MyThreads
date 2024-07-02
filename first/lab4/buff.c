#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void buffer_changes() {
    char *buffer = malloc(100);
    strcpy(buffer, "hello world");
    printf("buffer = %s\n", buffer);
    free(buffer);
    printf("buffer = %s\n", buffer);
    buffer = malloc(100);
    strcpy(buffer, "hello world");
    printf("buffer = %s\n", buffer);
    buffer += 50;
    free(buffer);
    printf("buffer = %s\n", buffer);
}

int main() {
    buffer_changes();
    return 0;
}

