#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE 4096
#define STACK_SIZE PAGE * 4
#define MAX_THREADS_COUNT 10

typedef void *(*start_routine_t)(void*);

typedef struct _mythread {
    int mythread_id;
    void *arg;
    start_routine_t start_routine;
    void *retval;
    int joined;
    int finished;
    int detached;
    void *stack;
} mythread_struct_t;

typedef mythread_struct_t* mythread_t;

typedef struct _mythread_cleanup {
    int thread_count;
    mythread_t thread_list[MAX_THREADS_COUNT]
} mythread_cleanup_t;

void *create_stack(off_t size, int mytid) {

    char stack_file[128];
    int stack_fd;
    void *stack;

    snprintf(stack_file, sizeof(stack_file), "stack-%d", mytid);

    stack_fd = open(stack_file, O_RDWR | O_CREAT, 0660);
    ftruncate(stack_fd, 0);
    ftruncate(stack_fd, size);

    stack = mmap(NULL, size, PROT_NONE, MAP_SHARED, stack_fd, 0);
    close(stack_fd);

    return stack;

}

void free_stack(void *stack, size_t size) {
    munmap(stack, size);
}

void cleanup_threads(mythread_cleanup_t *threads_cleanup) {

    int count = threads_cleanup->thread_count;

    while (count != 0) {

        for (int i = 0; i < threads_cleanup->thread_count; i++) {
            mythread_struct_t *thread = threads_cleanup->thread_list[i];
            if (thread->detached && thread != NULL) {
                free_stack(thread->stack, STACK_SIZE);
                thread = NULL;
                count--;
            }
            else if (!thread->detached && thread != NULL) {
                while (!thread->joined && thread != NULL) {
                    for (int j = 0; j < threads_cleanup->thread_count; j++) {
                        mythread_struct_t *thread_other = threads_cleanup->thread_list[j];
                        if (thread_other->detached && thread_other != NULL) {
                            free_stack(thread_other->stack, STACK_SIZE);
                            thread_other = NULL;
                            count--;
                        }
                    }
                }
                if (thread != NULL) {
                    free_stack(thread->stack, STACK_SIZE);
                    thread = NULL;
                    count--;
                }
            }
        }

    }

}

int thread_startup(void *arg) {

    mythread_t tid = (mythread_t)arg;
    mythread_struct_t *thread = tid;
    void *retval;

    printf("starting thread function for thread %d\n", thread->mythread_id);
    retval = thread->start_routine(thread->arg);

    thread->retval = retval;
    thread->finished = 1;

    //printf("waiting for join thread %d\n", thread->mythread_id);
    //if (!thread->detached) {
    //    while(!thread->joined) sleep(1);
    //    free_stack(thread->stack, STACK_SIZE);
    //}

    printf("thread function finished for thread %d\n", thread->mythread_id);

    return 0;

}

int mythread_create(mythread_t *mytid, void *start_routine, void *arg, mythread_cleanup_t *threads_cleanup) {

    mythread_struct_t *thread;
    int child_pid;
    void *child_stack;

    static int mythread_id = 0;
    mythread_id++;

    static int mythread_cleanup_id = 0;

    printf("created thread %d\n", mythread_id);

    child_stack = create_stack(STACK_SIZE, mythread_id);
    mprotect(child_stack + PAGE, STACK_SIZE - PAGE, PROT_READ | PROT_WRITE);
    memset(child_stack + PAGE, 0x7f, STACK_SIZE -  PAGE);

    thread = (mythread_struct_t*)(child_stack + STACK_SIZE - sizeof(mythread_struct_t));

    thread->mythread_id = mythread_id;
    thread->arg = arg;
    thread->start_routine = start_routine;
    thread->retval = NULL;
    thread->finished = 0;
    thread->joined = 0;
    thread->detached = 0;
    thread->stack = child_stack;

    child_stack = (void *)thread;

    //printf("child stack %p; mythread struct %p\n", child_stack, thread);

    child_pid = clone(thread_startup, child_stack, CLONE_VM | CLONE_FILES | CLONE_THREAD | CLONE_SIGHAND | SIGCHLD, thread);
    if (child_pid == -1) {
        printf("clone failed: %s\n", strerror(errno));
        free_stack(child_stack, STACK_SIZE);
        return -1;
    }

    threads_cleanup->thread_count = mythread_id;
    threads_cleanup->thread_list[mythread_cleanup_id] = thread;
    mythread_cleanup_id++;

    *mytid = thread;
    return 0;

}

void mythread_join(mythread_t mytid, void **retval) {
    mythread_struct_t *thread = mytid;

    printf("waiting finishing thread %d\n", thread->mythread_id);
    while (!thread->finished) sleep(1);
    printf("thread %d finished\n", thread->mythread_id);

    *retval = thread->retval;
    thread->joined = 1;
}

void mythread_detach(mythread_t mytid, void **retval) {
    mythread_struct_t *thread = mytid;

    while (!thread->finished) sleep(1);
    printf("finishing thread %d\n", thread->mythread_id);

    *retval = thread->retval;
    thread->detached = 1;

    //free_stack(thread->stack, STACK_SIZE);
}

void *mythread(void *arg) {
    
    char *str = (char *)arg;

    for (int i = 0; i < 5; i++) {
        printf("hello from my thread %s\n", str);
        sleep(1);
    }

    return "done";

}

int main() {
    mythread_t mytid1;
    void *retval1;
    mythread_t mytid2;
    void *retval2;

    mythread_cleanup_t threads_cleanup;

    printf("main %d %d %d\n", getpid(), getppid(), gettid());
    mythread_create(&mytid1, mythread, "ok", &threads_cleanup);
    mythread_create(&mytid2, mythread, "ok", &threads_cleanup);
    mythread_detach(mytid1, &retval1);
    mythread_join(mytid2, &retval2);
    //printf("main %d %d %d thread returned %s\n", getpid(), getppid(), gettid(), (char *)retval);

    cleanup_threads(&threads_cleanup);
    sleep(5);
    return 0;
}
