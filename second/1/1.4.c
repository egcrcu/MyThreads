#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int count = 0;
char* str = NULL;

void *thread_str(void *arg) {
    while(1) printf("Hello from thread function\n");
}

void *thread_count(void *arg) {
    //while(1) printf("%d\n", count++);
    while (1) count++;
}

void cleanup(void *arg) {
    //free(arg);
    //arg = NULL;
    free(str);
    str = NULL;
}

void *thread_malloc(void *arg) {
    str = malloc(14);
    strcpy(str, "Hello, World!");
    pthread_cleanup_push(cleanup, NULL);
    while(1) printf("%s\n", str);
    pthread_cleanup_pop(1);
}

int main() {

    pthread_t thread;

    pthread_create(&thread, NULL, thread_str, NULL);
    sleep(3);
    pthread_cancel(thread);
    //pthread_join(thread, NULL);

    pthread_create(&thread, NULL, thread_count, NULL);
    sleep(3);
    pthread_cancel(thread);
    //pthread_join(thread, NULL);

    pthread_create(&thread, NULL, thread_malloc, NULL);
    sleep(3);
    pthread_cancel(thread);
    //pthread_join(thread, NULL);

    if (str == NULL) {
        printf("Память успешно освобождена.\n");
    } else {
        printf("Ошибка: Память не была освобождена.\n");
    }

    pthread_exit(NULL);

    return 0;
}