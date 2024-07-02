#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char *env_var = getenv("MY_ENV_VAR");
    printf("env_var = %s\n", env_var);
    printf("&env_var = %p\n", &env_var);
    setenv("MY_ENV_VAR", "new_value", 1);
    env_var = getenv("MY_ENV_VAR");
    printf("env_var = %s\n", env_var);
    printf("&env_var = %p\n", &env_var);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        char *child_env_var = getenv("MY_ENV_VAR");
        printf("env_var = %s\n", env_var);
        printf("&env_var = %p\n", &env_var);
        printf("child_env_var = %s\n", child_env_var);
        printf("&child_env_var = %p\n", &child_env_var);
        exit(0);
    }
    
    else if (pid > 0) {
        char pid[50];
        sprintf(pid, "cat /proc/%d/maps", getpid());
        system(pid);
    }
    
    else {
        printf("Failed to create child process\n");
        return 1; 
    }
    
    return 0;
}
