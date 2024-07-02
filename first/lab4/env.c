#include <stdio.h>
#include <stdlib.h>

int main() {
    char *env_var = getenv("MY_ENV_VAR");
    printf("env_var = %s\n", env_var);
    printf("%p\n", &env_var);
    setenv("MY_ENV_VAR", "new_value", 1);
    env_var = getenv("MY_ENV_VAR");
    printf("env_var = %s\n", env_var);
    printf("%p\n", &env_var);
    char pid[50];
    sprintf(pid, "cat /proc/%d/maps", getpid());
    system(pid);
    return 0;
}
