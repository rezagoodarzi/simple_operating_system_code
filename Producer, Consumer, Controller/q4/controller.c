#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MEMORY_SIZE 100
#define HIGH_THRESHOLD 80
#define LOW_THRESHOLD 30

void execute_consumer() {
    execlp("./consumer", "./consumer", NULL);
    perror("exec failed");
    exit(EXIT_FAILURE);
}

int main() {
    int shm_fd;
    int *shared_memory;
    pid_t consumer_pid;
    int consumer_count = 0;

    shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, MEMORY_SIZE * sizeof(int));
    shared_memory = mmap(NULL, MEMORY_SIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (1) {
        int occupied_memory = 0;
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            if (shared_memory[i] != 0) {
                occupied_memory++;
            }
        }

        printf("Occupied Memory: %d\n", occupied_memory);

        if (occupied_memory > HIGH_THRESHOLD && consumer_count < 5) {
            consumer_count++;
            consumer_pid = fork();

            if (consumer_pid == 0) {
                execute_consumer();
            }
        } else if (occupied_memory < LOW_THRESHOLD && consumer_count > 1) {
            kill(consumer_pid, SIGTERM);
            wait(NULL);
            consumer_count--;
        }

        sleep(3);
    }

    shm_unlink("/shared_memory");

    return 0;
}
