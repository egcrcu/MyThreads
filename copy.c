#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_PATH_LEN 4096

typedef struct {
    char src[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];
} CopyTask;

void* copy_file(void* arg) {
    CopyTask* task = (CopyTask*)arg;

    int src_fd = open(task->src, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        pthread_exit(NULL);
    }

    int dest_fd = open(task->dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        pthread_exit(NULL);
    }

    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytesRead) != bytesRead) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            pthread_exit(NULL);
        }
    }

    close(src_fd);
    close(dest_fd);
    pthread_exit(NULL);
}

void copy_directory(const char* src, const char* dest) {
    DIR* dir = opendir(src);
    if (dir == NULL) {
        perror("Error opening source directory");
        return;
    }

    struct dirent* entry;
    struct stat statbuf;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[MAX_PATH_LEN];
        char dest_path[MAX_PATH_LEN];

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (lstat(src_path, &statbuf) == -1) {
            perror("Error getting file information");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            mkdir(dest_path, statbuf.st_mode);
            copy_directory(src_path, dest_path);
        } else if (S_ISREG(statbuf.st_mode)) {
            pthread_t tid;
            CopyTask* task = malloc(sizeof(CopyTask));
            strncpy(task->src, src_path, MAX_PATH_LEN);
            strncpy(task->dest, dest_path, MAX_PATH_LEN);
            pthread_create(&tid, NULL, copy_file, task);
            pthread_join(tid, NULL);
            free(task);
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* src = argv[1];
    const char* dest = argv[2];

    mkdir(dest, 0777);  // Create destination directory

    copy_directory(src, dest);

    return 0;
}
