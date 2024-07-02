#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
    printf("Созданный поток завершился\n");
    pthread_exit((void *)42);
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, NULL);

    void *result;
    pthread_join(thread, &result);

    printf("Получено значение из созданного потока: %d\n", (int)result);

    printf("Основной поток завершился\n");
    return 0;
}