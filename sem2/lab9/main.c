#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#define SUCCESS 0
#define FAILURE 1

const long int smallIter = 100000;
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
// crit sec
    if (param == NULL)
        pthread_exit(NULL);
    iterData_t localIterData = *((iterData_t*)param);
//
    int bigIter = 0;
    while (1)
    {
        for (long int i = localIterData.start + bigIter*smallIter*threadNum; i < localIterData.start + (bigIter+1)*smallIter*threadNum; i += threadNum)
        {
            localIterData.sum += 1.0/(i*4.0 + 1.0);
            localIterData.sum -= 1.0/(i*4.0 + 3.0);
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
// crit sec
    ((iterData_t*) param)->sum = localIterData.sum;
    //
    pthread_exit(param);
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
    double totalSum = 0;

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
        totalSum += partSum->sum;
    }
    printf("\npi      = %.16f\n", 4*totalSum);
    printf("real pi = %.16f\n", M_PI);

    freeRes(threads, threadsIterData);
    pthread_exit(SUCCESS);
}
