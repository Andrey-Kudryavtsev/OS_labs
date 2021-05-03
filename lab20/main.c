#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <glob.h>

#define ERROR -1
#define SUCCESS 0
#define FAILURE 1

int printMatch(char *pattern)
{
    glob_t *pglob;
    int globResult = glob(pattern, GLOB_NOCHECK | GLOB_MARK, NULL, pglob);
    if (globResult != SUCCESS)
    {
        perror("lab19.out: glob()");
        globfree(pglob);
        return ERROR;
    }

    size_t matchAmount = pglob->gl_pathc;
    for (int i = 0; i < matchAmount; ++i)
    {
        printf("%s\n", pglob->gl_pathv[i]);
    }
    globfree(pglob);
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    char pattern[LINE_MAX];
    printf("Enter the file pattern:\n");
    int scanfResult = scanf("%s", pattern);
    if (scanfResult == EOF)
    {
        perror("lab19.out: reading the pattern");
        exit(FAILURE);
    }

    int printMatchResult = printMatch(pattern);
    if (printMatchResult == ERROR)
    {
        exit(FAILURE);
    }

    exit(SUCCESS);
}
