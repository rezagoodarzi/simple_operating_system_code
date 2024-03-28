#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void execute_command(char *command) {
    pid_t pid;
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) == -1) {
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {

        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        system(command);
        exit(EXIT_SUCCESS);
    } else {

        close(pipe_fd[1]);

        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);

        char buffer[4096];
        ssize_t bytesRead;

        while ((bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Execute the command
    execute_command(argv[1]);

    return 0;
}
