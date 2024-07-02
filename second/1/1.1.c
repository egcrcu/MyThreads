#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int global = 0;

void *mythread(void *arg) {
    int thread_id = *((int *)arg);
	printf("(%d) mythread [%d %d %d]: Hello from mythread!\n", thread_id, getpid(), getppid(), gettid());
    int local = 0;
    static int local_static;
    const int local_const = 10;
    printf("local = %d\n", local);
    printf("global = %d\n", global);
    printf("&local = %p\n", &local);
    printf("&local_static = %p\n", &local_static);
    printf("&local_const = %p\n", &local_const);
    printf("&global = %p\n", &global);
    local++;
    global++;
    printf("local = %d\n", local);
    printf("global = %d\n\n\n", global);	
	return NULL;
}

int main() {
	pthread_t tid;
	int err;
	
	pthread_t threads[5];
	int targs[5];

	printf("\nmain [%d %d %d]: Hello from main!\n\n\n", getpid(), getppid(), gettid());

	
	for (int i = 0; i < 5; i++) {
	    targs[i] = i + 1;
	    err = pthread_create(&threads[i], NULL, mythread, &targs[i]);
	    if (err) {
	        printf("main: pthread_create() failed: %s\n", strerror(err));
	        return -1;
	    }
	}
	
	/*for (int i = 0; i < 5; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                perror("Ошибка при ожидании завершения потока");
                exit(EXIT_FAILURE);
            }
        }*/
        
        char pid[50];
        sprintf(pid, "cat /proc/%d/maps", getpid());
        system(pid);
        
        pthread_exit(NULL);
        //sleep(5);
	
	//if (pthread_join(tid, NULL) != 0) {
	//    perror("Ошибка при ожидании завершения потока");
	//    exit(EXIT_FAILURE);
        //}

	return 0;
}
