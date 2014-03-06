#include <stdio.h> /* printf(), perror() */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE */
#include <unistd.h> /* fork() */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait() */

int main() {
    // Create a new process:
    pid_t pid;
    pid = fork();

    // On error:
    if(pid == -1) {
        perror("Could not create new process using fork");
        exit(EXIT_FAILURE);
    }

    // In new process (child):
    else if(pid == 0) {
        printf("CHILD: Hello world!\n");
        exit(EXIT_SUCCESS);
    }

    // In the original process (parent):
    printf("PARENT: My child's pid is %d.\n", pid);

    // Wait for the child to complete.
    int exit_status;
    pid_t child_pid;
    child_pid = wait(&exit_status);
    printf("PARENT: My child %d exited with status %d.\n", child_pid, exit_status);

    return EXIT_SUCCESS;
}
