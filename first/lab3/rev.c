#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void create_reverse_dir(char *dir_name) {
  int len;
  
  for (int i = strlen(dir_name); i >= 0; i--) {
    if (dir_name[i] == '/') {
      len = strlen(dir_name) - i;
      break;
    }
  }
  
  char *rev_name = malloc(len + 1);
  int old_len = strlen(dir_name) - 1;
  int j;
  
  for (j = 0; j < len; j++) {
    rev_name[j] = dir_name[old_len];
    old_len--;
  }
  
  rev_name[j] = '\0';
  mkdir(rev_name, 0777);
}

int main(int argc, char **argv) {
  char *source_dir = argv[1];
  create_reverse_dir(source_dir);
  return 0;
}
