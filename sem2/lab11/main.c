#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SUCCESS 0
#define STRING_AMOUNT 50
#define MUTEX_AMOUNT 3

pthread_mutexattr_t mattr;
pthread_mutex_t mutexes[MUTEX_AMOUNT];

void initMutexes()
{
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
    for (int i = 0; i < MUTEX_AMOUNT; i++)
    {
        pthread_mutex_init(mutexes+i, &mattr);
    }
}

void destroyMutexes()
{
    for (int i = 0; i < MUTEX_AMOUNT; i++)
    {
        pthread_mutex_destroy(mutexes+i);
    }
}

void lock(int i, char *message)
{
    int code = pthread_mutex_lock(mutexes+i);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof(buf));
        fprintf(stderr, "%s: %d %s\n", message, i, buf);
        pthread_exit(NULL);
    }
}

void unlock(int i, char *message)
{
    int code = pthread_mutex_unlock(mutexes+i);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof(buf));
        fprintf(stderr, "%s: %d %s\n", message, i, buf);
        pthread_exit(NULL);
    }
}

void* sayHello(void *param)
{
    lock(1, "lock mutex");
    for (int i = 0; i < STRING_AMOUNT; i++)
    {
        lock(0, "lock mutex");
        unlock(1, "unlock mutex");
        printf("%s: Hello, my friends! i = %d\n", (char*) param, i);
        lock(2, "lock mutex");
        unlock(0, "unlock mutex");
        lock(1, "lock mutex");
        unlock(2, "unlock mutex");
    }
    unlock(1, "unlock mutex");

    pthread_exit(param);
}

int main(int argc, char **argv)
{
    pthread_t thread;
    char *parent = "PARENT";
    char *child = "CHILD";

    initMutexes();
    int code = pthread_create(&thread, NULL, sayHello, child);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof(buf));
        fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
        pthread_exit(SUCCESS);
    }

    sayHello(parent);

    code = pthread_join(thread, NULL);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof buf);
        fprintf(stderr, "%s: joining thread: %s\n", argv[0], buf);
        pthread_exit(NULL);
    }
    destroyMutexes();
    pthread_exit(SUCCESS);
}
