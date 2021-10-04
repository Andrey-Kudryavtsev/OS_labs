#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERROR -1
#define SUCCESS 0

void* cleanupHandler(void *param)
{
    write(1, "I'm dying...\n", 14);

    return param;
}

void* sayHello(void *param)
{
    pthread_cleanup_push(cleanupHandler, param);
    for (;;)
    {
        ssize_t result = write(1, "Hello, my friends!\n", 20);
        if (result == ERROR)
        {
            perror("writing to stdout");
            break;
        }
    }

    pthread_cleanup_pop(1);
    pthread_exit(param);
}

int main(int argc, char **argv)
{
    pthread_t thread;

    int code = pthread_create(&thread, NULL, sayHello, NULL);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof(buf));
        fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
    }

    sleep(2);
    pthread_cancel(thread);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof(buf));
        fprintf(stderr, "%s: cancelling thread: %s\n", argv[0], buf);
    }
    printf("STOP IT!!!\n");

    pthread_exit(SUCCESS);
}
