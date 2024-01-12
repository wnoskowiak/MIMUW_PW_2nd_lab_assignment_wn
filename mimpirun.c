/**
 * This file is for implementation of mimpirun program.
 * */

#include "mimpi_common.h"
#include "channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Function to set an environment variable to a specified integer value using setenv
int set_environment_variable_int(const char *name, int value) {
    if (name == NULL) {
        return -1; // Invalid input
    }

    // Convert integer to string
    char value_str[12];  // Assuming a 32-bit integer, so at most 11 digits plus null terminator
    snprintf(value_str, sizeof(value_str), "%d", value);

    // Set the environment variable
    if (setenv(name, value_str, 1) != 0) {
        return -1; // Error setting environment variable
    }

    return 0; // Success
}

int open_pipe_with_fd(int *pipe_fd, int specific_fd) {
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return -1;  // Error creating the pipe
    }

    // Duplicate specific_fd to one end of the pipe
    if (dup2(pipe_fd[1], specific_fd) == -1) {
        perror("dup2");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;  // Error duplicating file descriptor
    }

    // Close the duplicated file descriptor
    close(pipe_fd[1]);

    return 0;  // Success
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Użycie: %s <liczba_kopii> <ścieżka_do_programu> [argumenty...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Pobieranie argumentów z linii poleceń
    int n = atoi(argv[1]);
    char *prog = argv[2];

    // Przygotowanie argumentów do przekazania do programu prog
    char *args[argc - 2 + 1];  // +1 dla ostatniego elementu NULL w argumencie execvp

    // Kopiowanie argumentów od trzeciego
    for (int i = 3; i < argc; i++) {
        args[i - 3] = argv[i];
    }

    // Ostatni element musi być NULL
    args[argc - 3] = NULL;

    set_environment_variable_int("MIMPI_WORLD", n);

    channels_init();

    int pipefd[2];

    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(j!=i) {
                int value = (2*(16*j + i))+20;
                channel(pipefd);
                dup2(pipefd[0], value);
                dup2(pipefd[1], value+1);
                close(pipefd[0]);
                close(pipefd[1]);
            }
        }
    }

    channels_finalize();

    // Pętla uruchamiająca n kopii programu
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Błąd fork()");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {

            set_environment_variable_int("MIMPI_RANK", i);

            // Proces potomny
            execvp(prog, args);

            channels_init();

            for(int i=0; i<n; i++) {
                for(int j=0; j<n; j++) {
                    if(j!=i) {
                        int value = (2*(16*j + i))+20;
                        close(value);
                        close(value+1);
                    }
                }
            }

            channels_finalize();

            // W przypadku błędu execvp
            perror("Błąd execvp()");
            exit(EXIT_FAILURE);
        }
    }

    channels_init();

    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(j!=i) {
                int value = (2*(16*j + i))+20;
                close(value);
                close(value+1);
            }
        }
    }

    channels_finalize();

    // Czekanie na zakończenie wszystkich procesów potomnych
    for (int i = 0; i < n; i++) {
        int status;
        wait(&status);
    }

    return 0;
}

