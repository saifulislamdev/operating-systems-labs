#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ATTEMPTS 10 // The number of times the agent puts ingredients on the table

/* Function declarations */
void agent(void *arg);
void smoker(void *arg);

pthread_t agentThread, matchThread, paperThread, tobaccoThread;           // Threads
pthread_mutex_t agentLock, matchLock, paperLock, threadLock, tobaccoLock; // Mutex locks

void agent(void *arg)
{
    srand(time(NULL));

    for (size_t i = 1; i <= ATTEMPTS; i++)
    {
        pthread_mutex_lock(&threadLock); // Only thread out of the others running
        int randNum = rand() % 3 + 1;    // Pick a random number from 1-3

        if (randNum == 1)
        {
            // Put tobacco on table
            // Put paper on table
            printf("Agent put tobacco and paper on table\n");
            pthread_mutex_unlock(&matchLock); // Allow match smoker to go
        }
        else if (randNum == 2)
        {
            // Put tobacco on table
            // Put match on table
            printf("Agent put tobacco and match on table\n");
            pthread_mutex_unlock(&paperLock); // Allow paper smoker to go
        }
        else if (randNum == 3)
        {
            // Put match on table
            // Put paper on table
            printf("Agent put match and paper on table\n");
            pthread_mutex_unlock(&tobaccoLock); // Allow tobacco smoker to go
        }

        printf("Agent will go %lu more time\n", ATTEMPTS - i);
        printf("Agent sleep\n");
        pthread_mutex_unlock(&threadLock); // Allow appropriate smoker to go
        pthread_mutex_lock(&agentLock);    // Agent sleeps until smoker finishes
    }
}

void smoker(void *arg)
{
    char *smoker = (char *)arg;

    while (true)
    {
        /* Sleep right away */
        if (strcmp(smoker, "match") == 0)
            pthread_mutex_lock(&matchLock);
        else if (strcmp(smoker, "paper") == 0)
            pthread_mutex_lock(&paperLock);
        else if (strcmp(smoker, "tobacco") == 0)
            pthread_mutex_lock(&tobaccoLock);

        pthread_mutex_lock(&threadLock); // Only thread out of the others running

        if (strcmp(smoker, "match") == 0)
        {
            // Pick up paper
            // Pick up tobacco
            printf("Smoker with match puts match on table\n");
            // Smoke (but don't inhale).
        }
        else if (strcmp(smoker, "paper") == 0)
        {
            // Pick up match
            // Pick up tobacco
            printf("Smoker with paper puts paper on table\n");
            // Smoke (but don't inhale).
        }
        else if (strcmp(smoker, "tobacco") == 0)
        {
            // Pick up match
            // Pick up paper
            printf("Smoker with tobacco puts tobacco on table\n");
            // Smoke (but don't inhale).
        }
        pthread_mutex_unlock(&agentLock);  // Allow agent to go
        pthread_mutex_unlock(&threadLock); // Give back running state to agent
    }
}

int main()
{
    /* Initialize mutex locks */
    pthread_mutex_init(&agentLock, NULL);
    pthread_mutex_init(&matchLock, NULL);
    pthread_mutex_init(&paperLock, NULL);
    pthread_mutex_init(&threadLock, NULL);
    pthread_mutex_init(&tobaccoLock, NULL);

    pthread_create(&agentThread, NULL, agent, NULL);

    /* Agent goes first so perform locks */
    pthread_mutex_lock(&agentLock);
    pthread_mutex_lock(&matchLock);
    pthread_mutex_lock(&paperLock);
    pthread_mutex_lock(&tobaccoLock);

    /* Create smoker threads */
    pthread_create(&matchThread, NULL, smoker, "match");
    pthread_create(&paperThread, NULL, smoker, "paper");
    pthread_create(&tobaccoThread, NULL, smoker, "tobacco");

    pthread_join(agentThread, NULL);

    /* Terminate smoker threads once last smoker finishes */
    pthread_mutex_lock(&threadLock);
    pthread_cancel(matchThread);
    pthread_cancel(paperThread);
    pthread_cancel(tobaccoThread);
    pthread_mutex_unlock(&threadLock);

    /* Destroy mutex locks */
    pthread_mutex_destroy(&agentLock);
    pthread_mutex_destroy(&matchLock);
    pthread_mutex_destroy(&paperLock);
    pthread_mutex_destroy(&threadLock);
    pthread_mutex_destroy(&tobaccoLock);

    return 0;
}
