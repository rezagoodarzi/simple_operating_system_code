#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

struct SearchArgs {
    const char *dir;
    const char *query;
};

void *search(void *args) {
    struct SearchArgs *search_args = (struct SearchArgs *)args;
    DIR *dir = opendir(search_args->dir);

    if (dir == NULL) {
        pthread_exit(NULL);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR || entry->d_type == DT_UNKNOWN) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char subdir[2000];
            snprintf(subdir, sizeof(subdir), "%s/%s", search_args->dir, entry->d_name);
            struct SearchArgs *sub_args = malloc(sizeof(struct SearchArgs));
            sub_args->dir = subdir;
            sub_args->query = search_args->query;

            pthread_t thread;
            pthread_create(&thread, NULL, search, sub_args);
            pthread_join(thread, NULL);
            free(sub_args);
        } else {
            if (strstr(entry->d_name, search_args->query) != NULL) {
                printf("[+] Found: %s/%s\n", search_args->dir, entry->d_name);
            }
        }
    }

    closedir(dir);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4 || strcmp(argv[1], "search") != 0) {
        fprintf(stderr, "Usage: %s search <start_dir> <search_query>\n", argv[0]);
        return 1;
    }

    const char *start_dir = argv[2];
    const char *search_query = argv[3];

    pthread_t thread;
    struct SearchArgs *args = malloc(sizeof(struct SearchArgs));
    args->dir = start_dir;
    args->query = search_query;

    pthread_create(&thread, NULL, search, args);
    pthread_join(thread, NULL);
    free(args);

    return 0;
}