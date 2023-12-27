#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

void signal_handler(int sig) {
    printf("\nПолучен сигнал SIGINT со второго потока.\n");
}

void *thread1_function(void *arg) {
    sigset_t set;
    sigfillset(&set);
    sigprocmask(SIG_BLOCK, &set, NULL);

    while (1) {
        printf("thread 1\n");
        printf("%d\n", getpid());
        sleep(1);
    }

    return NULL;
}

void *thread2_function(void *arg) {
    struct sigaction sa;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);

    sigfillset(&sa.sa_mask);
    sigdelset(&sa.sa_mask, SIGINT);

    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    
    while (1) {
        printf("thread 2\n");
        printf("%d\n", getpid());
        sleep(1);
    }

    return NULL;
}

void *thread3_function(void *arg) {
    
    int sig;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    while (1) {
        sigwait(&set, &sig);
        printf("\nПолучен сигнал SIGQUIT с третьего потока.\n");
    }

    return NULL;
}

int main() {

    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_t thread1;
    pthread_create(&thread1, NULL, thread1_function, NULL);

    pthread_t thread2;
    pthread_create(&thread2, NULL, thread2_function, NULL);

    pthread_t thread3;
    pthread_create(&thread3, NULL, thread3_function, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;

}