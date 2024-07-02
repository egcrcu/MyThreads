#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void print_hello() {
  printf("Hello, World!\n");
}

void load_lib_and_run_func() {
  
  void *handle = dlopen("libhello_shared_dyn.so", RTLD_LAZY); //загружает в память динамическую библиотеку с полным именем, в handle ее описатель,
  //RTLD_LAZY - символы будут разрешены при первом обращении к ним (разрешение неопределенных символов в виде кода, который будет содержаться в .so)
  
  if (!handle) {
    printf("dlopen() failed: %s\n", dlerror()); //dlerror описывает, какая произошла ошибка
    return;
  }
  
  int (*dyn_print_hello)(void) = dlsym(handle, "print_hello_shared_dyn"); //возвращает указатель на функцию, импортируемую из библиотеки
  
  char *error = dlerror(); //получить сообщение о последней возникшей ошибке
  if (error != NULL) {
    printf("dlsym() failed: %s\n", dlerror());
    return;
  }
  
  dyn_print_hello(); //вызов функции
  dlclose(handle); //выгружает библиотеку из памяти
  
}

int main() {
  print_hello();
  load_lib_and_run_func();
  return 0;
}
