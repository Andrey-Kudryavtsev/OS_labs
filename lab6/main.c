#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <poll.h>

#define SUCCESS 0
#define FAILURE 1
#define ERROR -1

typedef struct t_LineInfo
{
    off_t offset;
    off_t length;
} t_LineInfo;

int makeTable(t_LineInfo **table, int filDes)
{
    int tableSize = 100;
    char buf[BUFSIZ];
    (*table) = (t_LineInfo*) malloc(tableSize * sizeof(t_LineInfo));
    if ((*table) == NULL)
    {
        perror("lab6.out: allocating memory for the table");
        return ERROR;
    }

    ssize_t bytes;
    int lineNum = 0;
    off_t length = 0;
    off_t offset = 0;
    while ((bytes = read(filDes, buf, BUFSIZ)) > 0)
    {
        for (ssize_t i = 0; i < bytes; ++i)
        {
            if (lineNum >= tableSize)
            {
                tableSize *= 2;
                (*table) = realloc((*table), tableSize * sizeof(t_LineInfo));
                if ((*table) == NULL)
                {
                    perror("lab6.out: reallocating memory for the table");
                    return ERROR;
                }
            }
            if (buf[i] == '\n')
            {
                t_LineInfo lineInfo = {offset, length};
                (*table)[lineNum] = lineInfo;
                offset += (length + 1);
                length = 0;
                lineNum++;
            } else
            {
                length++;
            }
        }
    }

    if (bytes == ERROR)
    {
        perror("lab6.out: read from the file while making table");
        return ERROR;
    }
    return lineNum;
}

int printLine(int filDes, int lineIdx, t_LineInfo *table)
{
    off_t lseekResult = lseek(filDes, table[lineIdx-1].offset, SEEK_SET);
    if (lseekResult == ERROR)
    {
        perror("lab6.out: change position in the file while printing line");
        return ERROR;
    }

    off_t lineLength = table[lineIdx-1].length;
    char line[lineLength + 1]; // +1 for '\0'
    ssize_t readResult = read(filDes, line, table[lineIdx-1].length);
    if (readResult == ERROR)
    {
        perror("lab6.out: reading selected line");
        return ERROR;
    }
    line[lineLength] = '\0';
    printf("%s\n", line);
    return SUCCESS;
}

int printFile(int filDes)
{
    ssize_t bytes;
    char buf[BUFSIZ];
    off_t lseekResult = lseek(filDes, 0, SEEK_SET);
    if (lseekResult == ERROR)
    {
        perror("lab6.out: change position in the file while printing file");
        return ERROR;
    }

    while((bytes = read(filDes, buf, BUFSIZ)) > 0)
    {
        int writeResult = write(1, buf, bytes);
        if (writeResult == ERROR)
        {
            perror("lab6.out: writing to the standard output");
            return ERROR;
        }
    }

    if (bytes == ERROR)
    {
        perror("lab6.out: read from the file while printing file");
        return ERROR;
    }
    return SUCCESS;
}

int searchInFile(int filDes, t_LineInfo *table, int lineNum)
{
    int lineIdx = 1;
    struct pollfd fd = {0, POLLIN, 0};

    while (lineIdx != 0)
    {
// wait until descriptor is ready
        int fdCount = poll(&fd, 1, 5000);
        if (fdCount == ERROR)
        {
            perror("lab6.out: poll error");
            return ERROR;
        }
        if (fdCount == 0)
        {
            printf("no input\n");
            int printFileResult = printFile(filDes);
            return printFileResult;
        }
// user input
        int scanfResult = scanf("%d", &lineIdx);
        if (scanfResult == EOF)
        {
            perror("lab6.out: reading the number");
            return ERROR;
        }
        int fflushResult = fflush(stdin);
        if (fflushResult == EOF)
        {
            perror("lab6.out: clearing stdin buffer");
            return ERROR;
        }
// check input
        if (lineIdx == 0)
        {
            printf("stop the application\n");
            continue;
        }
        if (lineIdx < 0 || lineIdx > lineNum)
        {
            printf("enter the number in the interval [0, %d]\n", lineNum);
            continue;
        }
// print requested line
        int printLineResult = printLine(filDes, lineIdx, table);
        if (printLineResult == ERROR)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

int main()
{
    t_LineInfo *table = NULL;
    int filDes = open("lab6.c", O_RDONLY);
    if (filDes == ERROR)
    {
        perror("lab6.out: opening the file");
        exit(FAILURE);
    }

    int lineNum = makeTable(&table, filDes);
    if (lineNum == ERROR)
    {
        free(table);
        close(filDes);
        exit(FAILURE);
    }

    int searchInFileResult = searchInFile(filDes, table, lineNum);
    if (searchInFileResult == ERROR)
    {
        free(table);
        close(filDes);
        exit(FAILURE);
    }

    free(table);
    int closeResult = close(filDes);
    if (closeResult == ERROR)
    {
        perror("lab6.out: closing the file");
        exit(FAILURE);
    }
    exit(SUCCESS);
}
