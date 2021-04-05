#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <poll.h>
#include <sys/mman.h>

#define STOP_APP 0
#define SUCCESS 0
#define FAILURE 1
#define ERROR -1
#define TIMEOUT 5000

typedef struct t_LineInfo
{
    off_t offset;
    off_t length;
} t_LineInfo;

int makeTable(t_LineInfo **table, char *filMap, int filSize)
{
    int tableSize = 100;
    (*table) = (t_LineInfo *) malloc(tableSize * sizeof(t_LineInfo));
    if ((*table) == NULL)
    {
        perror("lab7.out: allocating memory for the table");
        return ERROR;
    }

    int lineNum = 0;
    off_t length = 0;
    off_t offset = 0;
    for (int i = 0; i < filSize; ++i)
    {
        if (lineNum >= tableSize)
        {
            tableSize *= 2;
            (*table) = (t_LineInfo *) reallocf((*table), tableSize * sizeof(t_LineInfo));
            if ((*table) == NULL)
            {
                perror("lab7.out: reallocating memory for the table");
                return ERROR;
            }
        }
        if (filMap[i] == '\n')
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
    return lineNum;
}

int printLine(char *filMap, int lineIdx, t_LineInfo *table)
{
    off_t lineLength = table[lineIdx-1].length;
    off_t lineOffset = table[lineIdx-1].offset;
    char line[lineLength + 1]; // +1 for '\0'

    memcpy(line, filMap + lineOffset, lineLength);
    line[lineLength] = '\0';
    printf("%s\n", line);

    return SUCCESS;
}

int searchInFile(char *filMap, t_LineInfo *table, int lineNum)
{
    int lineIdx = 1;
    struct pollfd fd = {0, POLLIN, 0};

    while (lineIdx != STOP_APP)
    {
// wait until descriptor is ready
        int fdCount = poll(&fd, 1, TIMEOUT);
        if (fdCount == ERROR)
        {
            perror("lab7.out: poll error");
            return ERROR;
        }
        if (fdCount == 0)
        {
            printf("no input\n%s\n", filMap);
            return SUCCESS;
        }
// user input
        int scanfResult = scanf("%d", &lineIdx);
        if (scanfResult == EOF)
        {
            perror("lab7.out: reading the number");
            return ERROR;
        }
        int fflushResult = fflush(stdin);
        if (fflushResult == EOF)
        {
            perror("lab7.out: clearing stdin buffer");
            return ERROR;
        }
// check input
        if (lineIdx == STOP_APP)
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
        int printLineResult = printLine(filMap, lineIdx, table);
        if (printLineResult == ERROR)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

int freeResources(t_LineInfo *table, int filDes, char *filMap, int filSize)
{
    free(table);
    int munmapResult = munmap(filMap, filSize);
    if (munmapResult == ERROR)
    {
        perror("lab7.out: unmapping the file");
        close(filDes);
        return ERROR;
    }
    int closeResult = close(filDes);
    if (closeResult == ERROR)
    {
        perror("lab7.out: closing the file");
        return ERROR;
    }
    return SUCCESS;
}

int main()
{
    t_LineInfo *table = NULL;
    char *filMap = NULL;
    struct stat filStat;
    int filSize;
    int filDes;
    int lineNum;

    filDes = open("lab7.c", O_RDONLY);
    if (filDes == ERROR)
    {
        perror("lab7.out: opening the file");
        exit(FAILURE);
    }

    int fstatResult = fstat(filDes, &filStat);
    if (fstatResult == ERROR)
    {
        perror("lab7.out: getting file statistic");
        close(filDes);
        exit(FAILURE);
    }
    filSize = filStat.st_size;

    filMap = (char *) mmap(0, filSize, PROT_READ, MAP_SHARED, filDes, 0);
    if (filMap == MAP_FAILED)
    {
        perror("lab7.out: mapping the file");
        close(filDes);
        exit(FAILURE);
    }

    lineNum = makeTable(&table, filMap, filSize);
    if (lineNum == ERROR)
    {
        freeResources(table, filDes, filMap, filSize);
        exit(FAILURE);
    }

    int searchInFileResult = searchInFile(filMap, table, lineNum);
    if (searchInFileResult == ERROR)
    {
        freeResources(table, filDes, filMap, filSize);
        exit(FAILURE);
    }

    int freeResourcesResult = freeResources(table, filDes, filMap, filSize);
    if (freeResourcesResult == ERROR)
    {
        exit(FAILURE);
    }
    exit(SUCCESS);
}
