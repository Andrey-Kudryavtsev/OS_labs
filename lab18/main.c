#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>
#include <string.h>

#define ERROR -1
#define SUCCESS 0
#define FAILURE 1
#define BUFSIZE 128

extern int errno;

char *defaultDirEntries[] = {".", (char *) 0};

int printPermissions(struct stat fileStat)
{
    char permissions[11] = {0};

    switch (fileStat.st_mode & S_IFMT)
    {
        case S_IFREG:
            permissions[0] = '-';
            break;
        case S_IFDIR:
            permissions[0] = 'd';
            break;
        default:
            permissions[0] = '?';
            break;
    }

    permissions[1] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';

    permissions[4] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';

    permissions[7] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

    permissions[10] = '\0';
    printf("%s", permissions);

    return SUCCESS;
}
int printLinkNumber(struct stat fileStat)
{
    printf("\t%u", fileStat.st_nlink);

    return SUCCESS;
}

int printUserName(char *fileName, struct stat fileStat)
{
    errno = 0;
    struct passwd *uent = getpwuid(fileStat.st_uid);
    if (uent == NULL && errno != 0)
    {
        perror(fileName);
        return ERROR;
    }
    if (uent == NULL)
    {
        printf("\t%d", fileStat.st_uid);
    } else
    {
        printf("\t%s", uent->pw_name);
    }

    return SUCCESS;
}

int printGroupName(char *fileName, struct stat fileStat)
{
    errno = 0;
    struct group *gent = getgrgid(fileStat.st_gid);
    if (gent == NULL && errno != 0)
    {
        perror(fileName);
        return ERROR;
    }
    if (gent == NULL)
    {
        printf("\t%d", fileStat.st_gid);
    } else
    {
        printf("\t%s", gent->gr_name);
    }

    return SUCCESS;
}

int printSize(struct stat fileStat)
{
    if ((fileStat.st_mode & S_IFMT) == S_IFREG)
    {
        printf("\t%ld", fileStat.st_size);
    } else
    {
        printf("\t%s", "");
    }

    return SUCCESS;
}
int printTime(char *fileName, struct stat fileStat)
{
    char ctimeString[BUFSIZE];
    struct tm *ctime = localtime(&fileStat.st_ctime);
    if (ctime == NULL)
    {
        perror(fileName);
        return ERROR;
    }
    int strftimeResult = strftime(ctimeString, BUFSIZE, "%b %d %G %H:%M", ctime);
    if (strftimeResult == 0)
    {
        fprintf(stderr, "%s: not enough size of buffer\n", fileName);
        return ERROR;
    }
    printf("\t%s", ctimeString);

    return SUCCESS;
}

int printName(char *fileName)
{
    printf("\t%s\n", basename(fileName));

    return SUCCESS;
}

int printFileStat(char *fileName, struct stat fileStat)
{
    printPermissions(fileStat);
    printLinkNumber(fileStat);
    printUserName(fileName, fileStat);
    printGroupName(fileName, fileStat);
    printSize(fileStat);
    printTime(fileName, fileStat);
    printName(fileName);
}

int main(int argc, char *argv[])
{
    char **dirEntries = (argc > 1) ? &(argv[1]) : defaultDirEntries;

    for (int i = 0; dirEntries[i] != NULL; ++i)
    {
        struct stat fileStat;
        char *fileName = dirEntries[i];
        int lstatResult = lstat(fileName, &fileStat);
        if (lstatResult == ERROR)
        {
            perror(fileName);
        } else
        {
            printFileStat(fileName, fileStat);
        }
    }

    exit(SUCCESS);
}
