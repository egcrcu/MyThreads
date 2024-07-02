#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

char* create_reverse_dir(char *dir_name) {
  dir_name = strrchr(dir_name, '/') + 1;
  char *rev_name = malloc(strlen(dir_name) + 1);
  int i, j;
  for (i = strlen(dir_name) - 1, j = 0; i >= 0; i--, j++) {
    rev_name[j] = dir_name[i];
  }
  rev_name[j] = '\0';
  mkdir(rev_name, 0777);
  return rev_name;
}

void copy_reverse(char *source, char *dest_dir) {
  int fd1, fd2;
  char *buffer;
  int size;
  int i;

  fd1 = open(source, O_RDONLY);
  size = lseek(fd1, 0, SEEK_END);
  buffer = malloc(size);
  lseek(fd1, 0, SEEK_SET);
  read(fd1, buffer, size);

  char *file_name = strrchr(source, '/') + 1;
  char *new_file_name = malloc(strlen(dest_dir) + strlen(file_name) + 1);
  int len = strlen(file_name);
  for (i = 0; i < len; i++) new_file_name[len-i-1] = file_name[i];
  new_file_name[len] = '\0';

  char *new_path = malloc(strlen(dest_dir) + strlen(new_file_name) + 2);
  sprintf(new_path, "%s/%s", dest_dir, new_file_name);
  
  fd2 = open(new_path, O_WRONLY | O_CREAT, 0666);
  for (i = size - 1; i >= 0; i--) write(fd2, &buffer[i], 1);

  free(new_file_name);
  free(new_path);
  free(buffer);
  close(fd1);
  close(fd2);
}

int main(int argc, char **argv) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char *source_dir = argv[1];
  char *dest_dir = create_reverse_dir(source_dir);
  dir = opendir(source_dir);

  while ((entry = readdir(dir)) != NULL) {
    char *name = entry->d_name;
    char *path = malloc(strlen(source_dir) + strlen(name) + 2);
    sprintf(path, "%s/%s", source_dir, name);
    stat(path, &statbuf);
    if (S_ISREG(statbuf.st_mode)) copy_reverse(path, dest_dir);
    free(path);
  }

  closedir(dir);
  free(dest_dir);

  return 0;
}
