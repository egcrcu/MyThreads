#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
    static char str[] = "hello";
    printf("Созданный поток завершился\n");
    pthread_exit(str);
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, NULL);

    void *result;
    pthread_join(thread, &result);

    printf("Получено сообщение из созданного потока: %s\n", (char*)result);

    printf("Основной поток завершился\n");
    return 0;
}