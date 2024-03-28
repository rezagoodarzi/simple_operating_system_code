#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void read_numbers(const char *filename, int *numbers, int *num_numbers) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", num_numbers);
    printf("Total numbers: %d\n", *num_numbers);

    for (int i = 0; i < *num_numbers; ++i) {
        fscanf(file, "%d", &numbers[i]);
        printf("Read number: %d\n", numbers[i]);
    }

    fclose(file);
}

void mean_min(const int *numbers, int num_numbers, float *mean, float *minimum) {
    *mean = 0;
    *minimum = numbers[0];

    for (int i = 0; i < num_numbers; ++i) {
        *mean += numbers[i];
        if (numbers[i] < *minimum) {
            *minimum = numbers[i];
        }
    }

    *mean /= num_numbers;
}

int main() {
    const char *filename = "numbers.txt";

    int num_numbers;
    int numbers[100];
    float mean, minimum;

    pid_t child_pid1, child_pid2;
    int pipe_fd[2];
    int pipe2_fd[2];
    if (pipe(pipe_fd) == -1) {
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe2_fd) == -1) {
        exit(EXIT_FAILURE);
    }
    child_pid1 = fork();

    if (child_pid1 == -1) {
        exit(EXIT_FAILURE);
    }

    if (child_pid1 == 0) {
    // First child process (reading numbers)
    close(pipe_fd[0]); 

    read_numbers(filename, numbers, &num_numbers);
   
    write(pipe_fd[1], numbers, sizeof(numbers));
    write(pipe_fd[1], &num_numbers, sizeof(num_numbers));

    close(pipe_fd[1]); 
    exit(EXIT_SUCCESS);
    }

    wait(NULL);

    child_pid2 = fork();

    if (child_pid2 == -1) {
        exit(EXIT_FAILURE);
    }

    if (child_pid2 == 0) {
        // Second child process (calculating statistics)
        close(pipe_fd[1]); 
        read(pipe_fd[0], numbers, sizeof(numbers));
        read(pipe_fd[0], &num_numbers, sizeof(num_numbers));

        close(pipe_fd[0]); 
        
        mean_min(numbers, num_numbers, &mean, &minimum);

        write(pipe2_fd[1], &mean, sizeof(mean));
        write(pipe2_fd[1], &minimum, sizeof(minimum));

        close(pipe2_fd[1]); 
        exit(EXIT_SUCCESS);
    }

        wait(NULL); 

        close(pipe_fd[1]); 
        close(pipe2_fd[1]); 

        read(pipe2_fd[0], &mean, sizeof(mean));
        read(pipe2_fd[0], &minimum, sizeof(minimum));

        close(pipe2_fd[0]); 

        printf("Mean = %.2f, Min = %.2f\n", mean, minimum);

    return 0;
}
