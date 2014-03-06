#include <stdlib.h> /* EXIT_SUCCESS */
#include <stdio.h> /* printf() */
#include <unistd.h> /* getpid() */

int main() {
    pid_t my_pid;
    my_pid = getpid();

    printf("My process id (pid) is %d.\n", my_pid);
}
