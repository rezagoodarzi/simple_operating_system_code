#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

#define M 3
#define N 3
#define K 3

int m1[M][N] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
int m2[N][K] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
int m3[M][K];

void multiply_row(int row, int result[M][K]) {
    printf("Process %d starting...\n", row);

    for (int i = 0; i < K; ++i) {
        result[row][i] = 0;
        for (int j = 0; j < N; ++j) {
            result[row][i] += m1[row][j] * m2[j][i];
        }
    }

    printf("Process %d finished.\n", row);
}

int main() {
    int shm_fd;
    int (*result)[K];

    // Create shared memory
    shm_fd = shm_open("/matrix_multiplication", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(int[M][K]));
    result = mmap(0, sizeof(int[M][K]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for (int i = 0; i < M; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            multiply_row(i, result);
            exit(EXIT_SUCCESS);
        } 
    }

    for (int i = 0; i < M; ++i) {
        wait(NULL);
    }

    printf("Result Matrix:\n");
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < K; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    munmap(result, sizeof(int[M][K]));
    close(shm_fd);
    shm_unlink("/matrix_multiplication");

    return 0;
}
