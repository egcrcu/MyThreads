#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>

char* create_reverse_dir(char *dir_path) {
  //char *dir_end = strrchr(dir_path, '/') + 1;
  char *dir_end = basename(dir_path) - 1;
  int len = strlen(dir_end);
  char *rev_name = malloc(len+1);
  for (int i = 0; i < len; i++) rev_name[i] = dir_end[len - i - 1];
  rev_name[len] = '\0';

  char *parent_dir = strdup(dir_path);
  //char *last_slash = strrchr(parent_dir, '/');
  char *last_slash = basename(parent_dir) - 1;
  if (last_slash != NULL) *last_slash = '\0';
  
  char *new_dir_path = malloc(strlen(parent_dir) + strlen(rev_name) + 2);
  sprintf(new_dir_path, "%s/%s", parent_dir, rev_name);
  mkdir(new_dir_path, 0777);
  
  free(parent_dir);
  free(rev_name);
  return new_dir_path;
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

  //char *file_name = strrchr(source, '/') + 1;
  char *file_name = basename(source);
  int len = strlen(file_name);
  char *new_file_name = malloc(len+1);
  for (i = 0; i < len; i++) new_file_name[i] = file_name[len-i-1];
  new_file_name[len] = '\0';

  char *new_path = malloc(strlen(dest_dir) + strlen(new_file_name) + 2);
  sprintf(new_path, "%s/%s", dest_dir, new_file_name);

  struct stat st;
  stat(source, &st);

  //fd2 = open(new_path, O_WRONLY | O_CREAT, 0666);
  fd2 = open(new_path, O_WRONLY | O_CREAT, st.st_mode);
  for (i = size - 1; i >= 0; i--) write(fd2, &buffer[i], 1);
  
  chmod(new_path, st.st_mode);

  free(new_file_name);
  free(new_path);
  free(buffer);
  close(fd1);
  close(fd2);
}

void copy_recursive(char *source_dir, char *dest_dir) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char *name;
  char *path;
  char *new_dest_dir;
  char *rev_path;

  dir = opendir(source_dir);

  while ((entry = readdir(dir)) != NULL) {
    name = entry->d_name;
    path = malloc(strlen(source_dir) + strlen(name) + 2);
    sprintf(path, "%s/%s", source_dir, name);
    stat(path, &statbuf);
    
    if (S_ISREG(statbuf.st_mode)) copy_reverse(path, dest_dir);
    
    else if (S_ISDIR(statbuf.st_mode)) {
      if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        free(path);
        continue;
      }
      rev_path = malloc(strlen(dest_dir) + strlen(name) + 2);
      sprintf(rev_path, "%s/%s", dest_dir, name);
      new_dest_dir = create_reverse_dir(rev_path);
      copy_recursive(path, new_dest_dir);
      free(new_dest_dir);
    }
    
    free(path);
  }
  closedir(dir);
}

int main(int argc, char **argv) {
  char *source_dir = argv[1];
  char *dest_dir = create_reverse_dir(source_dir);
  copy_recursive(source_dir, dest_dir);
  free(dest_dir);
  return 0;
}

