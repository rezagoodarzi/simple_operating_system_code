#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void search_file(const char *filename, const char *query, int* occurrences_pipe) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[256];
    int occurrences = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, query) != NULL) {
            occurrences++;
        }
    }

    printf("%s: %d\n", query, occurrences);

    fclose(file);
    write(occurrences_pipe[1], &occurrences, sizeof(int));
    close(occurrences_pipe[1]);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file_to_search> <query1> <query2> <query3> ...\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int total_Queries = argc - 2;

    int child;
    int occurrences_pipe[2];
    
    int pip = pipe(occurrences_pipe);
    if (pip == -1){
        fprintf(stderr,"error creating pipe\n");
        return -1;
    }

    for (int i = 0; i < total_Queries; i++) {
        child = fork();
        if (child == 0) {
            close(occurrences_pipe[0]);
            search_file(filename, argv[i + 2],occurrences_pipe);
            exit(0);
        } else if (child < 0) {
            fprintf(stderr, "Error creating child process\n");
            return 1;
        }
    }

    for (int i = 0; i < total_Queries; i++) {
        wait(NULL);
    }

    int total = 0;
    int occur;
    for (int i = 0; i < total_Queries; i++) {
        read(occurrences_pipe[0],&occur,sizeof(int));
        total +=occur;
    }

    printf("Total occurrences: %d\n", total);

    return 0;
}   