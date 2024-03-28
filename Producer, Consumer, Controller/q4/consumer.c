#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MEMORY_SIZE 10

sem_t *empty, *full, *mutex;
int *shared_memory;
int consumer_count = 1;

void *consumer(void *arg) {
    while (1) {
        sem_wait(full);
        sem_wait(mutex);

        int value = shared_memory[(consumer_count - 1) % MEMORY_SIZE];
        printf("Consumer %d: Removed %d from the memory.\n", *((int *)arg), value);
        consumer_count++;

        sem_post(mutex);
        sem_post(empty);

        sleep(2);
    }

    return NULL;
}

int main() {
    int i;
    pthread_t cons_thread1, cons_thread2;

    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MEMORY_SIZE * sizeof(int));
    shared_memory = mmap(NULL, MEMORY_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    empty = sem_open("/empty", O_CREAT | O_RDWR, 0666, MEMORY_SIZE);
    full = sem_open("/full", O_CREAT | O_RDWR, 0666, 0);
    mutex = sem_open("/mutex", O_CREAT | O_RDWR, 0666, 1);

    int consumer_id1 = 1, consumer_id2 = 2;
    pthread_create(&cons_thread1, NULL, consumer, (void *)&consumer_id1);
    pthread_create(&cons_thread2, NULL, consumer, (void *)&consumer_id2);

    pthread_join(cons_thread1, NULL);
    pthread_join(cons_thread2, NULL);

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/empty");
    sem_unlink("/full");
    sem_unlink("/mutex");
    shm_unlink("/shared_memory");
    return 0;
}
