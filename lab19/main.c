#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <glob.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0
#define FAILURE 1

int isPatternCorrect(char *pattern)
{
// check for slashes and change '\n' with '\0' if needed
    int i;
    for (i = 0; pattern[i] != '\0'; ++i)
    {
        if (pattern[i] == '\n')
        {
            pattern[i] = '\0';
            break;
        }
        if (pattern[i] == '/')
        {
            fprintf(stderr, "lab19: '/' in pattern is not allowed\n");
            return ERROR;
        }
    }
// check for length
    if (i == (PATH_MAX + 2 - 1))
    {
        printf("Pattern is too long\n");
        return ERROR;
    }

    return SUCCESS;
}

int getPattern(char *pattern)
{
    char *fgetsResult = fgets(pattern, PATH_MAX + 2, stdin);
    int ferrorResult = ferror(stdin);
    if (ferrorResult != SUCCESS)
    {
        perror("lab19.out: reading the pattern");
        return ERROR;
    }
    if (fgetsResult == NULL)
    {
        printf("lab19.out: no pattern was given\n");
        return ERROR;
    }
    int isPatternCorrectResult = isPatternCorrect(pattern);
    if (isPatternCorrectResult != SUCCESS)
    {
        return ERROR;
    }

    return SUCCESS;
}

int printMatch(char *pattern)
{
    glob_t pglob;
    int globResult = glob(pattern, GLOB_NOCHECK | GLOB_MARK, NULL, &pglob);
    if (globResult != SUCCESS)
    {
        perror("lab19.out: glob()");
        globfree(&pglob);
        return ERROR;
    }
    size_t matchAmount = pglob.gl_pathc;
    printf("matchAmount=%d\n", matchAmount);
    for (int i = 0; i < matchAmount; ++i)
    {
        printf("%s\n", pglob.gl_pathv[i]);
    }
    globfree(&pglob);
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    char pattern[PATH_MAX + 2]; // +2 for '\0' and length check
    int getPatternResult = getPattern(pattern);
    if (getPatternResult == ERROR)
    {
        exit(FAILURE);
    }
    int printMatchResult = printMatch(pattern);
    if (printMatchResult == ERROR)
    {
        exit(FAILURE);
    }

    exit(SUCCESS);
}
