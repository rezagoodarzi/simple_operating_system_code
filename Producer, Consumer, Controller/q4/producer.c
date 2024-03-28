#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MEMORY_SIZE 10

sem_t *empty, *full, *mutex;
int *shared_memory;
int producer_count = 1;

void *producer(void *arg) {
    while (1) {
        sleep(rand() % 4 + 1);  

        sem_wait(empty);
        sem_wait(mutex);

        int value = rand() % 20 + 1;
        shared_memory[producer_count % MEMORY_SIZE] = value;

        printf("Producer %d: Put %d in the memory.\n", *((int *)arg), value);
        producer_count++;

        sem_post(mutex);
        sem_post(full);
    }

    return NULL;
}


int main() {
    int i;
    pthread_t prod_thread;

    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MEMORY_SIZE * sizeof(int));
    shared_memory = mmap(NULL, MEMORY_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    empty = sem_open("/empty", O_CREAT | O_RDWR, 0666, MEMORY_SIZE);
    full = sem_open("/full", O_CREAT | O_RDWR, 0666, 0);
    mutex = sem_open("/mutex", O_CREAT | O_RDWR, 0666, 1);

    int producer_id = 1;
    pthread_create(&prod_thread, NULL, producer, (void *)&producer_id);

    pthread_join(prod_thread, NULL);

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/empty");
    sem_unlink("/full");
    sem_unlink("/mutex");
    shm_unlink("/shared_memory");
    return 0    ;
}
