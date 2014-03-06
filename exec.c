#include <stdio.h> /* printf(), perror() */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h> /* fork(), execlp() */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait() */

int main() {
    pid_t pid;
    // Create new process.
    pid = fork();
    // On error: exit
    if(pid == -1) {
        perror("Could not create new process using fork");
        exit(EXIT_FAILURE);
    }
    // On child: replace self with "echo hello world" process
    else if(pid == 0) {
        execlp("/bin/echo", "echo", "hello", "world", NULL);
        // this should never execute:
        printf("There was an error in execlp!\n");
        exit(EXIT_FAILURE);
    }
    // On parent: wait for child to terminate
    int exit_status;
    wait(&exit_status);
    printf("PARENT: subprocess exited with status %d.\n", exit_status);
    return EXIT_SUCCESS;
}
