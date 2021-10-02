#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SUCCESS 0

const int iterations = 10000000;
int iterPerThread;

typedef struct iterData
{
    int i;
    double sum;
} iterData_t;

void *calcSum(void *param)
{
    iterData_t localIterData = *((iterData_t*) param);
    for (int i = localIterData.i; i < (localIterData.i + iterPerThread); i++)
    {
        localIterData.sum += 1.0/(i*4.0 + 1.0);
        localIterData.sum -= 1.0/(i*4.0 + 3.0);
    }
    ((iterData_t*) param)->sum = localIterData.sum;
    pthread_exit(param);
}

int readInput(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    if (atoi(argv[1]) < 1)
    {
        printf("Wrong argument\n");
        exit(EXIT_FAILURE);
    }
    return atoi(argv[1]);
}

void freeRes(pthread_t *threads, iterData_t *threadsIterData)
{
    free(threads);
    free(threadsIterData);
}

int main(int argc, char **argv)
{
    int threadNum = readInput(argc, argv);

    double totalSum = 0;
    pthread_t *threads = (pthread_t*) malloc(threadNum * sizeof(pthread_t));
    iterData_t *threadsIterData = (iterData_t*) malloc(threadNum * sizeof(iterData_t));
    iterPerThread = iterations / threadNum;

    for (int i = 0; i < threadNum; i++)
    {
        threadsIterData[i].i = i*iterPerThread;
        threadsIterData[i].sum = 0;
        int code = pthread_create(&(threads[i]), NULL, calcSum, &(threadsIterData[i]));
        if (code != SUCCESS)
        {
            char buf[256];
            strerror_r(code, buf, sizeof buf);
            fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
            freeRes(threads, threadsIterData);
            pthread_exit(NULL);
        }
    }

    for (int i = 0; i < threadNum; i++)
    {
        iterData_t *partSum;
        int code = pthread_join(threads[i], (void**) &partSum);
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
    printf("pi = %.16f\n", 4*totalSum);
    printf("pi = %.16f\n", M_PI);

    freeRes(threads, threadsIterData);
    pthread_exit(EXIT_SUCCESS);
}
