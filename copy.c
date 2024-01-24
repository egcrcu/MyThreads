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
} copy_task;

void* copy_file(void* arg) {
    copy_task* task = (copy_task*)arg;

    int src_fd = open(task->src, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        free(arg);
        pthread_exit(NULL);
    }

    struct stat statbuf;

    if (fstat(src_fd, &statbuf) == -1) {
        perror("Error getting source file information");
        close(src_fd);
        free(arg);
        pthread_exit(NULL);
    }

    int dest_fd = open(task->dest, O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        free(arg);
        pthread_exit(NULL);
    }

    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            free(arg);
            pthread_exit(NULL);
        }
    }

    close(src_fd);
    close(dest_fd);
    free(arg);
    pthread_exit(NULL);
}

void* copy_directory(void* arg) {
    copy_task* task = (copy_task*)arg;

    char abs_src[MAX_PATH_LEN];
    char abs_dest[MAX_PATH_LEN];
    int count = 0;

    const char *task_src = task->src;
    const char *task_dest = task->dest;

    realpath(task_src, abs_src);
    realpath(task_dest, abs_dest);

    if (strcmp(abs_src, abs_dest) == 0) pthread_exit(NULL);

    char* str = "";

    if (strlen(abs_src) <= strlen(abs_dest)) {
        while (1) {
            str = strrchr(abs_dest, '/');
            count = str - abs_dest;
            if (count == 0) break;
            strncpy(abs_dest, abs_dest, count);
            abs_dest[count] = '\0';
            //printf("%s\n", abs_dest);
            if (strcmp(abs_src, abs_dest) == 0) {
                printf("Cannot copy a directory, '%s', into itself, '%s'\n", task->src, task->dest);
                pthread_exit(NULL);
            }
        }
    }

    DIR* dir = opendir(task->src);
    if (dir == NULL) {
        perror("Error opening source directory");
        pthread_exit(NULL);
    }

    long name_max = pathconf(task->src, _PC_NAME_MAX);
    long entry_size = sizeof(struct dirent) + name_max + 1;
    //struct dirent* entry;
    struct dirent *entry = malloc(entry_size);
    struct dirent *result;

    //while ((entry = readdir(dir)) != NULL) {
    while (readdir_r(dir, entry, &result) == 0 && result != NULL) {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        //char src_path[MAX_PATH_LEN];
        //char dest_path[MAX_PATH_LEN];

        //snprintf(src_path, sizeof(src_path), "%s/%s", task->src, entry->d_name);
        //snprintf(dest_path, sizeof(dest_path), "%s/%s", task->dest, entry->d_name);

        size_t src_path_len = strlen(task->src) + strlen(entry->d_name) + 2;
        size_t dest_path_len = strlen(task->dest) + strlen(entry->d_name) + 2;

        char *src_path = malloc(src_path_len);
        char *dest_path = malloc(dest_path_len);

        snprintf(src_path, src_path_len + 1, "%s/%s", task->src, entry->d_name);
        snprintf(dest_path, dest_path_len + 1, "%s/%s", task->dest, entry->d_name);
        
        struct stat statbuf;

        if (lstat(src_path, &statbuf) == -1) {
            perror("Error getting file information");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {

            if (mkdir(dest_path, statbuf.st_mode) == -1) {
                perror("Error creating destination directory");
                closedir(dir);
                pthread_exit(NULL);
            }

            copy_task* subtask = malloc(sizeof(copy_task));
            strncpy(subtask->src, src_path, MAX_PATH_LEN);
            strncpy(subtask->dest, dest_path, MAX_PATH_LEN);

            pthread_t tid;
            pthread_create(&tid, NULL, copy_directory, subtask);
            pthread_detach(tid);

        } else if (S_ISREG(statbuf.st_mode)) {
            pthread_t tid;
            copy_task* file_task = malloc(sizeof(copy_task));
            strncpy(file_task->src, src_path, MAX_PATH_LEN);
            strncpy(file_task->dest, dest_path, MAX_PATH_LEN);
            pthread_create(&tid, NULL, copy_file, file_task);
            pthread_detach(tid);
        }

        free(src_path);
        free(dest_path);

    }

    closedir(dir);
    free(entry);
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* src = argv[1];
    const char* dest = argv[2];

    struct stat statbuf;
    
    if (lstat(src, &statbuf) == -1) {
        perror("Error getting source directory information");
        exit(EXIT_FAILURE);
    }

    if (mkdir(dest, statbuf.st_mode) == -1) {
        perror("Error creating destination directory");
        exit(EXIT_FAILURE);
    }

    copy_task* task = malloc(sizeof(copy_task));
    strncpy(task->src, src, MAX_PATH_LEN);
    strncpy(task->dest, dest, MAX_PATH_LEN);

    pthread_t tid;
    pthread_create(&tid, NULL, copy_directory, task);
    pthread_join(tid, NULL);

    return 0;
}
