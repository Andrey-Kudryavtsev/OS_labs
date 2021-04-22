#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR -1
#define SUCCESS 0
#define FAILURE 1

extern char **environ;

int execvpe(char *file, char *const argv[], char *envp[])
{
    char **saved_envp = environ;
    environ = envp;
    execvp(file, argv);

    perror("lab11.out: exec the file");
    environ = saved_envp;
    return ERROR;
}

int main(int argc, char *argv[])
{
    char *new_envp[4] = {"RANDOM_VAR1=operating", "RANDOM_VAR2=system", "PATH=/home/students/19200/a.kudryavtsev2/lab11", (char *) 0};
    char *new_argv[2] = {"tst.out", (char *) 0};

    execvpe(new_argv[0], new_argv, new_envp);
    exit(FAILURE);
}
