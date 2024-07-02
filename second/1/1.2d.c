#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg) {
    printf("Идентификатор потока: %lu\n", pthread_self());
    //pthread_detach(pthread_self());
    pthread_exit(NULL);
}

int main() {

    while (1) {
    
        pthread_t thread;
        pthread_create(&thread, NULL, thread_function, NULL);
        
        /*pthread_t thread;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &attr, thread_function, NULL);
        pthread_attr_destroy(&attr);*/
        
    }

    printf("Основной поток завершился\n");
    return 0;
}