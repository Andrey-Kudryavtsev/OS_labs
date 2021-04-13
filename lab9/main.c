#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0
#define CHILD_PROCESS 0
#define FAILURE 1
#define ERROR -1

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if (pid == ERROR)
    {
        perror("lab9.out: fork the process");
        exit(FAILURE);
    }
    else if (pid == CHILD_PROCESS)
    {
        execl("/bin/cat", "cat", "TheEmpireStrikesBackScript", (char *) 0);
        perror("lab9.out: exec the programm");
        exit(FAILURE);
    }
    else if (pid != CHILD_PROCESS)
    {
        int waitResult = wait(NULL);
        if (waitResult == ERROR)
        {
            perror("lab9.out: waiting for child process");
            exit(FAILURE);
        }
        printf("No, I'm your father\n");
    }
    exit(SUCCESS);
}
