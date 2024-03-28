#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 5000

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

char buffer[BUFFER_SIZE];
size_t buffer_size = 0;

void* producer(void* arg);
void* consumer(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file_path> <destination_file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t producer_thread, consumer_thread;
    pthread_mutex_init(&buffer_mutex, NULL);
    printf("Customer %s \n",argv[0]);
    printf("Customer %s \n",argv[1]);
    printf("Customer %s \n",argv[2]);

    pthread_create(&producer_thread, NULL, producer, (void*)argv[1]);
    pthread_create(&consumer_thread, NULL, consumer, (void*)argv[2]);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&buffer_mutex);

    return 0;
}

void* producer(void* arg) {
    const char* source_file_path = (const char*)arg;

    FILE* source_file = fopen(source_file_path, "rb");
    if (source_file == NULL) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    size_t read_size;
    while ((read_size = fread(buffer, 1, BUFFER_SIZE, source_file)) > 0) {
        pthread_mutex_lock(&buffer_mutex);
        buffer_size = read_size;
        printf("Read Size: %zu\n", read_size);
        pthread_mutex_unlock(&buffer_mutex);
    }

    fclose(source_file);
    return NULL;
}

void* consumer(void* arg) {
    const char* destination_file_path = (const char*)arg;

    FILE* destination_file = fopen(destination_file_path, "wb");
    if (destination_file == NULL) {
        perror("Error opening destination file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pthread_mutex_lock(&buffer_mutex);

        if (buffer_size > 0) {
            fwrite(buffer, 1, buffer_size, destination_file);
            buffer_size = 0;
        } else {
            pthread_mutex_unlock(&buffer_mutex);
            break;  
        }

        pthread_mutex_unlock(&buffer_mutex);
    }

    fclose(destination_file);

    return NULL;
}
