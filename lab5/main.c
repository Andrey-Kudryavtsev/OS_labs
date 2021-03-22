#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SUCCESS 0
#define FAILURE 1
#define ERROR -1

typedef struct t_LineInfo
{
    off_t offset;
    off_t lenght;
} t_LineInfo;

int makeTable(t_LineInfo **table, int filDes)
{
    int tableSize = 100;
    char buf[BUFSIZ];
    (*table) = (t_LineInfo*) malloc(tableSize * sizeof(t_LineInfo));
    if ((*table) == NULL)
    {
        perror("allocating memory for the table");
        return ERROR;
    }

    ssize_t readedBytes;
    int lineNum = 0;
    off_t lenght = 0;
    off_t offset = 0;
    while ((readedBytes = read(filDes, buf, BUFSIZ)) > 0)
    {
        for (ssize_t i = 0; i < readedBytes; ++i)
        {
            if (lineNum >= tableSize)
            {
                tableSize *= 2;
                (*table) = realloc((*table), tableSize * sizeof(t_LineInfo));
                if ((*table) == NULL)
                {
                    perror("reallocating memory for the table");
                    return ERROR;
                }
            }
            if (buf[i] == '\n')
            {
                t_LineInfo lineInfo = {offset, lenght};
                (*table)[lineNum] = lineInfo;
                offset += (lenght + 1);
                lenght = 0;
                lineNum++;
            } else
            {
                lenght++;
            }
        }
    }

    if (readedBytes == ERROR)
    {
        perror("reading from the file");
        return ERROR;
    }
    return lineNum;
}
int main()
{
    t_LineInfo *table = NULL;
    int filDes = open("lab5.c", O_RDONLY);
    if (filDes == ERROR)
    {
        perror("opening the file");
        exit(FAILURE);
    }

    int lineNum = makeTable(&table, filDes);
    if (lineNum == ERROR)
    {
        free(table);
        close(filDes);
        exit(FAILURE);;
    }

    int lineIdx = 0;
    while (1)
    {
        int scanfResult = scanf("%d", &lineIdx);
        if (scanfResult == EOF)
        {
            perror("reading the number");
            free(table);
            close(filDes);
            exit(FAILURE);
        }
        if (lineIdx == 0)
        {
            break;
        }
        if (lineIdx < 0 || lineIdx > lineNum)
        {
            printf("enter the number in the interval [0, %d]\n", lineNum);
            continue;
        }
        off_t lseekResult = lseek(filDes, table[lineIdx-1].offset, SEEK_SET);
        if (lseekResult == -1)
        {
            perror("change position in the file");
            free(table);
            close(filDes);
            exit(FAILURE);
        }
        char buf[table[lineIdx-1].lenght + 1];
        ssize_t readResult = read(filDes, buf, table[lineIdx-1].lenght);
        if (readResult == ERROR)
        {
            perror("reading selected line");
            free(table);
            close(filDes);
            exit(FAILURE);
        }
        buf[table[lineIdx-1].lenght] = '\0';
        printf("%s\n", buf);
    }

    free(table);
    int closeResult = close(filDes);
    if (closeResult == ERROR)
    {
        perror("closing the file");
        exit(FAILURE);
    }
    exit(SUCCESS);
}
