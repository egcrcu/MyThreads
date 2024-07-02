#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global_var = 1;

int main() {
    int local_var = 2;
    printf("Адрес локальной переменной: %p, значение: %d\n", &local_var, local_var);
    printf("Адрес глобальной переменной: %p, значение: %d\n", &global_var, global_var);
    printf("PID: %d\n", getpid());
    
    pid_t pid = fork();
    
    if (pid == -1) {
        printf("Ошибка при порождении процесса\n");
        return 1;
    } 
    
    else if (pid == 0) {
        printf("PID дочернего процесса: %d, PPID: %d\n", getpid(), getppid());
        printf("Адрес локальной переменной: %p, значение: %d\n", &local_var, local_var);
        printf("Адрес глобальной переменной: %p, значение: %d\n", &global_var, global_var);
        local_var = 3;
        global_var = 4;
        printf("Новое значение локальной переменной: %d, %p\n", local_var, &local_var);
        printf("Новое значение глобальной переменной: %d, %p\n", global_var, &global_var);
        char pid[50];
        printf("\n\n\n");
        sprintf(pid, "cat /proc/%d/maps", getpid());
        system(pid);
        printf("\n\n\n");
        exit(5);
    }
    
    else {
        printf("PID родительского процесса: %d\n", getpid());
        printf("Содержимое локальной переменной в родительском процессе: %d, %p\n", local_var, &local_var);
        printf("Содержимое глобальной переменной в родительском процессе: %d, %p\n", global_var, &global_var);
        char pid[50];
        printf("\n\n\n");
        sprintf(pid, "cat /proc/%d/maps", getpid());
        system(pid);
        printf("\n\n\n");
        sleep(30);
        int status;
        pid_t child_pid = wait(&status);
        if (child_pid == -1) {
            printf("Ошибка при ожидании завершения дочернего процесса\n");
            return 1;
        } else {
            if (WIFEXITED(status)) {
                printf("Дочерний процесс завершился нормально, код завершения: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Дочерний процесс завершился сигналом, номер сигнала: %d\n", WTERMSIG(status));
            }
        }
    }
    
    return 0;
}

