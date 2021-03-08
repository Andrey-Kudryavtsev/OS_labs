#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SUCCESS 0
#define FAILURE 1
#define ERROR -1

//extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;
    char *date;
    int result = putenv("TZ=America/Los_Angeles"); // изменение переменной среды TZ
    if (result != SUCCESS)                                  // обработка ошибки функции putenv()
    {
        perror("lab2.out/main.c:17: error");        // вывод информации об ошибке в стандартный поток ошибок
        exit(FAILURE);
    }

    time(&now);                                             // в переменую now записываем время в секундах с полуночи 1 января 1970
    if (now == ERROR)
    {
        perror("lab2.out/main.c:24: error");
        exit(FAILURE);
    }

    date = ctime(&now);
    printf("%s", date); // функция ctime() конвертирует количество секунд в переменной time
                                // в локальное время в виде строки из 26 символов
    sp = localtime(&now);       // делает то же, что и ctime(), но возвращает указатель на структуру sp
    if (sp == NULL)
    {
        perror("lab2.out/main.c:34 error");
        exit(FAILURE);
    }
    printf("%d/%d/%02d %d:%02d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, sp->tm_sec,
        tzname[sp->tm_isdst]);

    exit(SUCCESS);
}
