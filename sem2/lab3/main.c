#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERROR -1
#define SUCCESS 0
#define NUM_THREADS 4
#define STRING_AMOUNT 4

void* saySomethingNice(void *speach)
{
    for (int i = 0; i < STRING_AMOUNT; i++)
    {
        ssize_t result = write(1, ((char **) speach)[i], strlen(((char **) speach)[i]));
        if (result == ERROR)
        {
            perror("writing to stdout");
            break;
        }
    }

    pthread_exit(speach);
}

int main(int argc, char **argv)
{
    pthread_t thread[4];
    char *speach[4][4] = {{"We're no strangers to love\n",
                                  "You know the rules and so do I\n",
                                  "A full commitment's what I'm thinking of\n",
                                  "You wouldn't get this from any other guy\n"},

                          {"Never gonna give you up, never gonna let you down\n",
                                  "Never gonna run around and desert you\n",
                                  "Never gonna make you cry, never gonna say goodbye\n",
                                  "Never gonna tell a lie and hurt you\n"},

                          {"We've known each other for so long\n",
                                  "Your heart's been aching but you're too shy to say it\n",
                                  "Inside we both know what's been going on\n",
                                  "We know the game and we're gonna play it\n"},

                          {"I just wanna tell you how I'm feeling\n",
                                  "Gotta make you understand\n",
                                  "And if you ask me how I'm feeling\n",
                                  "Don't tell me you're too blind to see\n"}};

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int code = pthread_create(&(thread[i]), NULL, saySomethingNice, speach[i]);
        if (code != SUCCESS)
        {
            char buf[256];
            strerror_r(code, buf, sizeof(buf));
            fprintf(stderr, "%s: creating thread: %s\n", argv[0], buf);
        }
    }

    pthread_exit(SUCCESS);
}
