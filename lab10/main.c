#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERROR -1
#define FAILURE 1
#define SUCCESS 0
#define CHILD_PROCESS 0
#define NOT_TERMINATED_NORMALLY 0

int main(int argc, char *argv[])
{
    int status = 0;
    pid_t pid;

    if (argc < 2)
    {
        printf("not enough arguments\n");
        exit(FAILURE);
    }

    pid = fork();
    if (pid == ERROR)
    {
        perror("lab10.out: fork the process");
        exit(FAILURE);
    } else if (pid == CHILD_PROCESS)
    {
        execvp(argv[1], &argv[1]);
        perror("lab10.out: exec the program");
        exit(FAILURE);
    } else if (pid != CHILD_PROCESS)
    {
        int waitResult = wait(&status);
        if (waitResult == ERROR)
        {
            perror("lab10.out: waiting for child process");
            exit(FAILURE);
        }
        if (WIFEXITED(status) != NOT_TERMINATED_NORMALLY)
        {
            printf("Child process has ended. Exit code: %d\n", WEXITSTATUS(status));
        }
    }
    exit(SUCCESS);
}
