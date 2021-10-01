#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define SUCCESS 0
#define FAILURE 1

const long int smallIter = 10000000;
int threadNum;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
int latestIter = 0;
int stop = 0;

typedef struct iterData
{
    int start;
    double sum;
} iterData_t;

void stopExec(int signal)
{
    stop = 1;
}

void *calcSum(void *param)
{
    iterData_t *iterData = (iterData_t*) param;
    int bigIter = 0;
    while (1)
    {
        for (long int i = iterData->start + bigIter*smallIter*threadNum; i < iterData->start + (bigIter+1)*smallIter*threadNum; i += threadNum)
        {
            iterData->sum += 1.0/(i*4.0 + 1.0);
            iterData->sum -= 1.0/(i*4.0 + 3.0);
        }
        pthread_barrier_wait(&barrier);

        pthread_mutex_lock(&mutex);
        if (stop == 1 && bigIter == latestIter)
        {
            pthread_mutex_unlock(&mutex);
            break;
        } else
        {
            bigIter++;
            if (latestIter < bigIter)
                latestIter = bigIter;
        }
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(iterData);
}

int readInput(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        exit(FAILURE);
    }
    if (atoi(argv[1]) < 1)
    {
        printf("Wrong argument\n");
        exit(FAILURE);
    }
    return atoi(argv[1]);
}

void freeRes(pthread_t *threads, iterData_t *threadsIterData)
{
    free(threads);
    free(threadsIterData);
    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);
}

int main(int argc, char **argv)
{
// initializing
    threadNum = readInput(argc, argv);
    double globSum = 0;
    int code = pthread_barrier_init(&barrier, NULL, threadNum);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof buf);
        fprintf(stderr, "%s: initializing barrier: %s\n", argv[0], buf);
        exit(FAILURE);
    }

    code = pthread_mutex_init(&mutex, NULL);
    if (code != SUCCESS)
    {
        char buf[256];
        strerror_r(code, buf, sizeof buf);
        fprintf(stderr, "%s: initializing mutex: %s\n", argv[0], buf);
        pthread_barrier_destroy(&barrier);
        exit(FAILURE);
    }

    signal(SIGINT, stopExec);

    pthread_t *threads = (pthread_t*) malloc(threadNum * sizeof(pthread_t));
    iterData_t *threadsIterData = (iterData_t*) malloc(threadNum * sizeof(iterData_t));

// creating threads
    for (int i = 0; i < threadNum; i++)
    {
        threadsIterData[i].start = i;
        threadsIterData[i].sum = 0;
        code = pthread_create(&(threads[i]), NULL, calcSum, &(threadsIterData[i]));
        if (code != SUCCESS)
        {
            char buf[256];
            strerror_r(code, buf, sizeof buf);
            fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
            freeRes(threads, threadsIterData);
            pthread_exit(NULL);
        }
    }

// joining threads and calculate pi
    for (int i = 0; i < threadNum; i++)
    {
        iterData_t *partSum;
        code = pthread_join(threads[i], (void**) &partSum);
        if (code != SUCCESS)
        {
            char buf[256];
            strerror_r(code, buf, sizeof buf);
            fprintf(stderr, "%s: joining thread: %s\n", argv[0], buf);
            freeRes(threads, threadsIterData);
            pthread_exit(NULL);
        }
        globSum += partSum->sum;
    }
    printf("\npi = %.16f\n", 4*globSum);

    freeRes(threads, threadsIterData);
    pthread_exit(SUCCESS);
}
