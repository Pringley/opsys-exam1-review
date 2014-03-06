---
title: OpSys Exam 1 Review
author: Ben Pringle
date: March 6, 2014
geometry: margin=1in
numbersections: true
toc: true
include-before: |
    This work is licensed under a
    [Creative Commons Attribution 4.0 International
    License](http://creativecommons.org/licenses/by/4.0/).
...

# Basic Terms

Operating System (OS)

:   The **OS** is the specialized software that manages communication between
    normal programs and the hardware.

    For example, files are stored on a hard drive, which is a hardware device.
    Instead of directly querying the hard drive, programs can simply call the
    `open()` function, which is implemented by the operating system.

    The operating system provides many **services**, such as:

    -   Program execution: load programs into memory and run them

    -   I/O operations: interact with input/output devices such as hard disk,
        network connection

    -   Filesystem manipulation: read/write files, directories

Memory

:   **Memory** is a hardware device used to store data and programs used by the
    computer. Typically "memory" refers to *random access memory* (RAM). This
    is distinct from "storage" (which refers to the hard drive).

    Programs are stored as a set of *instructions* in memory. These
    instructions are interpreted and executed by the CPU.

Central Processing Unit (CPU)

:   A **CPU** is a piece of hardware with two main components:

    -   The Arithmetic Logic Unit (ALU) is capable of performing arithmetic
        instructions (such as add, subtract, multiply) and logical instruction
        (such as and, or, not).

    -   The Control Unit (CU) connects the ALU and the memory. When executing a
        program, it loads instructions one by one from the memory, then asks
        the ALU to perform arithmetic if necessary.

Process

:   The CPU will often need to run more than one program at once. For example,
    a user might have a web browser and a text editor open at the same time.

    A **process** is a currently executing instance of a program. The CPU can
    only execute one process at a time, so it needs to *multitask*, switching
    between jobs (processes) without waiting for each to finish.

    Each process has an identifying number, known as process id or `pid`. This
    number is assigned by the operating system when the process is created.

    (Although processes are executed concurrently, they are different from
    threads. A process is much more heavyweight and can in fact contain many
    threads. Threads are not on this exam.)

# System Calls in C on Linux

## `getpid()`

Each process has an identifying number, known as process id or `pid`. This
number is assigned by the operating system when the process is created. A
program can access its pid using **`getpid()`**:

>   **getpid()** returns the current process id as a pid\_t.

An example is shown below:

```c
#include <stdlib.h> /* EXIT_SUCCESS */
#include <stdio.h> /* printf() */
#include <unistd.h> /* getpid() */

int main() {
    pid_t my_pid;
    my_pid = getpid();

    printf("My process id (pid) is %d.\n", my_pid);
}
```

Possible output is shown below:

```
My process id (pid) is 87976.
```

## `fork()`, `wait()`, and `waitpid()`

Programs sometimes want to create new processes themselves. For example, Google
Chrome creates a new process to handle each tab, which allows them to render in
parallel and helps prevent errors in one tab from breaking the rest. Another
example is the terminal program, which creates a new process for every command
you run.

New processes are created using **`fork()`**.

>   **pid_t fork()**
>
>   Creates a new process that is an exact copy of the current process.
>
>   It takes no arguments and returns a process id of type pid\_t:
>
>   -   *In the original process* (parent): return the pid of the new process.
>   -   *In the new process* (child): return 0.  -   *On error*: return -1.

The created process is known as a **child process**, while the original process
is called the **parent process**.

The child process is an *exact copy* of the parent. They have the same
variables, stack, heap, and current point of execution (how far through the
program it has run).

**Important**: The parent and child do not *share* anything after it's all
copied.  If you change a variable in the child process, **the parent's variable
is unchanged**. Similarly, if you change a variable in the parent process, the
child is unchanged.

*Note*: A new process created using `fork()` is *always* a copy of the
original. To run another program as a subprocess, see [the `exec()`
section](#exec).

It is good practice for the parent process to wait for the child to exit before
exiting itself. This is done using **`wait()`**.

>   **pid_t wait(int \*exit_status)**
>
>   Pauses execution of the current process until a child process finishes.
>
>   It takes a pointer to an integer as an argument, and stores the exit status
>   of the child process in that integer. It returns a pid\_t with the pid of
>   the child process that terminated.

If a parent process exits before its child, the child is called an **orphan
process**. This is avoided by using `wait()`.

If a child process exits and the parent never calls `wait()`, then the child
becomes a **zombie process**. (The operating system will keep the child around
in a not-alive not-dead state in case the parent ever decides to call
`wait()`.)

A full example is shown below:

```c
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
```

One possible output of this program:

```
PARENT: My child's pid is 85959.
CHILD: Hello world!
PARENT: My child 85959 exited with status 0.
```

However, remember that processes are run *concurrently*! The order in which the
instructions "interleave" is unspecified. This means the output could just as
easily switch the order of the first two lines:

```
CHILD: Hello world!
PARENT: My child's pid is 85959.
PARENT: My child 85959 exited with status 0.
```

The last line will always be last, though, since it only executes after the
child has completed.

If the parent wants to *check* if a child has terminated without actually
waiting, it can use **`waitpid()`** with `WNOHANG`:

>   **pid_t waitpid(pid_t pid, int \*exit_status, int options)**
>
>   waitpid() is a more general version of wait().
>
>   Parameters:
>
>   -   **pid** specifies which child process to wait for. If 0, wait for any
>       child.
>
>   -   **exit_status** is a pointer to an integer. The exit status of the
>       child will be stored in that integer.
>
>   -   **options** allow the user to specify certain settings for how
>       waitpid() should work. (example: WNOHANG makes waitpid()
>       *nonblocking*).
>
>   Returns the pid of the child process that terminated. If WNOHANG was
>   specified, then it could also return 0, which means no process has
>   terminated.

A **blocking** function (such as `wait()`) will pause the program's execution
until successful completion -- that is, it will *actually wait* for the child
process to complete.

A **nonblocking** function (such as `waitpid()` with `WNOHANG`) will return
immediately, whether or not the child process has terminated.

The code snippet below shows how to use `waitpid()` in a nonblocking way.

```c
#include <stdio.h> /* printf(), perror() */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait(), WNHOHANG */

/*
 * check_children() - return 1 if any children have terminated, 0 if not
 */
int check_children() {
    pid_t pid;
    int exit_status;

    // Check if any process has terminated, nonblocking.
    pid = waitpid(0, &exit_status, WNOHANG);
    if(pid == 0) {
        // All children are still running.
        printf("No processes have terminated yet.\n");
        return 0;
    }
    else if (pid == -1) {
        // There was an error :(
        perror("waitpid experienced an error");
        return 0;
    }
    else {
        // A child process has terminated!
        printf("Process %d terminated.\n", pid);
        return 1;
    }
}
```

## `exec()` {#exec}

>   **int execlp(const char* file, const char* arg0, ...)**
>
>   Replace the current process with the program in **file**. Parameters are
>   used as arguments.

Example:

```c
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
```

Output:

```
hello world
PARENT: subprocess exited with status 0.
```

## `pipe()`, `dup2()`

>   **int pipe(int fds[2])**
>
>   Create a one-directional *pipe*. This function edits the fds parameter,
>   setting fds[1] to the write end of the pipe and settings fds[0] to the read
>   end of the pipe.

>   **int dup2(int fd, int newfd)**
>
>   Duplicate the file descriptor **fd**. The new descriptor is specified by
>   **newfd**.

## `open()`, `read()`, `write()`, `close()`

### File Descriptors (`stdin`, `stdout`, `stderr`)

## `opendir()`, `readdir()`, `closedir()`

## `signal()`, `kill()`

# CPU Scheduling

Terms:

-   **preemption**
-   **turnaround time**
-   **wait time**

## Algorithms

### FCFS -- First Come, First Served

### SJF -- Shortest Job First

### RR -- Round Robin

### Priority
