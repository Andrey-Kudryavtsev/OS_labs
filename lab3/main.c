#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define FAILURE 1
#define SUCCESS 0
#define ERROR -1

int main(int argc, char **argv)
{
    long int uid, euid;
    FILE *file = NULL;

    int iterationNum = 2;
    for (int i = 0; i < iterationNum; ++i)
    {
        uid = getuid();
        euid = geteuid();
        printf("Real user id:\t\t%ld\n"
               "Effective user id:\t%ld\n",
               uid, euid);
        file = fopen("input.txt", "r");
        if (file == NULL)
        {
            perror("lab3.out incurred an error");
            exit(FAILURE);
        }
        int fcloseResult = fclose(file);
        if (fcloseResult == EOF)
        {
            perror("lab3.out incurred an error");
            exit(FAILURE);
        }

        int seteuidResult = seteuid(uid);
        if (seteuidResult == ERROR)
        {
            perror("lab3.out incurred an error");
            exit(FAILURE);
        }
    }
    exit(SUCCESS);
}
