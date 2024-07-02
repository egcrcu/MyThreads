#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct my_struct {
    int int_value;
    char *string_value;
};

void *thread_function(void *arg) {
    struct my_struct *data = (struct my_struct *)arg;
    printf("Значение int: %d\n", data->int_value);
    printf("Строка: %s\n", data->string_value);
    return NULL;
}

int main() {

    struct my_struct my_data;
    my_data.int_value = 42;
    my_data.string_value = "Hello, World!";

    pthread_t thread;
    
    /*pthread_t thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);*/
    
    if (pthread_create(&thread, NULL, thread_function, &my_data) != 0) {
        perror("Ошибка при создании потока");
        return 1;
    }
    
    //sleep(1);
    pthread_join(thread, NULL);

    return 0;
}
