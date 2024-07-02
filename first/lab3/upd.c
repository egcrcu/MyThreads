#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <errno.h>

char* create_reverse_dir(char *dir_path) {
  char *dir_end = basename(dir_path) - 1;
  int len = strlen(dir_end);
  //char *rev_name = malloc(len+1);
  char rev_name[len+1];
  //if (rev_name == NULL) {
  //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}
  for (int i = 0; i < len; i++) rev_name[i] = dir_end[len - i - 1];
  rev_name[len] = '\0';

  //char *parent_dir = strdup(dir_path);
  char parent_dir[strlen(dir_path)];
  strcpy(parent_dir, dir_path);
  if (parent_dir == NULL) {
    fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  char *last_slash = basename(parent_dir) - 1;
  if (last_slash != NULL) *last_slash = '\0';

  //char *new_dir_path = malloc(strlen(parent_dir) + strlen(rev_name) + 2);
  char new_dir_path[strlen(parent_dir) + strlen(rev_name) + 2];
  //if (new_dir_path == NULL) {
  //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}
  sprintf(new_dir_path, "%s/%s", parent_dir, rev_name);

  //if (mkdir(new_dir_path, 0777) == -1) {
  //  fprintf(stderr, "Не удалось создать каталог: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}
  
  struct stat st;
  stat(dir_path, &st);

  if (mkdir(new_dir_path, st.st_mode) == -1) {
    fprintf(stderr, "Не удалось создать каталог: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  chmod(new_dir_path, st.st_mode);


  //free(parent_dir);
  //free(rev_name);
  char *result = strdup(new_dir_path);
  if (result == NULL) {
    fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return result;
}

void copy_reverse(char *source, char *dest_dir) {
  int fd1, fd2;
  //char *buffer;
  int size;
  int i;

  fd1 = open(source, O_RDONLY);
  if (fd1 == -1) {
    fprintf(stderr, "Не удалось открыть файл: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  size = lseek(fd1, 0, SEEK_END);
  if (size == -1) {
    fprintf(stderr, "Не удалось получить размер файла: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  //buffer = malloc(size);
  //char buffer[size];
  //if (buffer == NULL) {
  //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}

  //lseek(fd1, 0, SEEK_SET);
  //if (read(fd1, buffer, size) == -1) {
  //  fprintf(stderr, "Не удалось прочитать данные из файла: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}

  char *file_name = basename(source);
  int len = strlen(file_name);
  //char *new_file_name = malloc(len+1);
  char new_file_name[len+1];
  //if (new_file_name == NULL) {
  //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}
  for (i = 0; i < len; i++) new_file_name[i] = file_name[len-i-1];
  new_file_name[len] = '\0';

  //char *new_path = malloc(strlen(dest_dir) + strlen(new_file_name) + 2);
  char new_path[strlen(dest_dir) + strlen(new_file_name) + 2];
  //if (new_path == NULL) {
  //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
  //  exit(EXIT_FAILURE);
  //}
  sprintf(new_path, "%s/%s", dest_dir, new_file_name);

  struct stat st;
  stat(source, &st);

  fd2 = open(new_path, O_WRONLY | O_CREAT, st.st_mode);
  if (fd2 == -1) {
    fprintf(stderr, "Не удалось создать файл: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  //for (i = size - 1; i >= 0; i--) write(fd2, &buffer[i], 1);
  
  int bytes_left = size;
  int block_size = 2;
  char buffer[block_size];
  while (bytes_left > 0) {
    int bytes_to_read = (bytes_left < block_size) ? bytes_left : block_size;

    if (lseek(fd1, -bytes_to_read, SEEK_CUR) == -1 && errno != EINVAL) {
      fprintf(stderr, "Не удалось переместить указатель файла: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    int bytes_read = 0;
    int result;
    while (bytes_read < bytes_to_read) {
      result = read(fd1, buffer + bytes_read, bytes_to_read - bytes_read);
      if (result == -1) {
        fprintf(stderr, "Не удалось прочитать данные из файла: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (result == 0) {
        break;
      }
      bytes_read += result;
    }

    if (lseek(fd1, -bytes_to_read, SEEK_CUR) == -1) {
      fprintf(stderr, "Не удалось переместить указатель файла: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    int a, b;
    char temp;
    for (a = 0, b = bytes_to_read - 1; a < b; a++, b--) {
      temp = buffer[a];
      buffer[a] = buffer[b];
      buffer[b] = temp;
    }

    if (write(fd2, buffer, bytes_to_read) == -1) {
      fprintf(stderr, "Не удалось записать данные в файл: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    bytes_left -= bytes_to_read;
  }
  chmod(new_path, st.st_mode);
  close(fd1);
  close(fd2);
}

void copy_recursive(char *source_dir, char *dest_dir) {
  DIR *dir;
  struct dirent *entry;
  struct stat statbuf;
  char *name;
  //char *path;
  char *new_dest_dir;
  //char *rev_path;

  dir = opendir(source_dir);
  if (dir == NULL) {
    fprintf(stderr, "Не удалось открыть каталог: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL) {
    name = entry->d_name;
    //path = malloc(strlen(source_dir) + strlen(name) + 2);
    char path[strlen(source_dir) + strlen(name) + 2];
    //if (path == NULL) {
    //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
    //  exit(EXIT_FAILURE);
    //}
    sprintf(path, "%s/%s", source_dir, name);
    stat(path, &statbuf);

    if (S_ISREG(statbuf.st_mode)) copy_reverse(path, dest_dir);

    else if (S_ISDIR(statbuf.st_mode)) {
      if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        //free(path);
        continue;
      }
      //rev_path = malloc(strlen(dest_dir) + strlen(name) + 2);
      char rev_path[strlen(dest_dir) + strlen(name) + 2];
      //if (rev_path == NULL) {
      //  fprintf(stderr, "Не удалось выделить память: %s\n", strerror(errno));
      //  exit(EXIT_FAILURE);
      //}
      sprintf(rev_path, "%s/%s", dest_dir, name);
      new_dest_dir = create_reverse_dir(rev_path);
      copy_recursive(path, new_dest_dir);
      free(new_dest_dir);
    }

    //free(path);
  }
  //free(new_dest_dir);
  closedir(dir);
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 2) {
    printf("Некорректное число аргументов\n");
    return 1;
  }
  char *source_dir = argv[1];
  char *dest_dir = create_reverse_dir(source_dir);
  copy_recursive(source_dir, dest_dir);
  free(dest_dir);
  return 0;
}

