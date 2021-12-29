#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "sem.h"

#define ATTEMPTS 10 // The number of times the agent puts ingredients on the table

int main()
{
    pid_t pid;
    FILE *fp1, *fp2, *fp3, *fp4, *fp5; // File pointers
    int N; // Total attempts
    int attemptsLeft;
    int flag1, flag2, flag3, flag4; // Used to indicate when each process should terminate
    int status;
    srand(time(NULL));

    // Initialize the number of attempts
    fp5 = fopen("attempt.txt", "w+");
    attemptsLeft = ATTEMPTS;
    fprintf(fp5, "%d\n", attemptsLeft);
    fclose(fp5);

    int agent = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (agent == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    int lock = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (lock == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    int smokerMatch = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (smokerMatch == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    int smokerPaper = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (smokerPaper == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    int smokerTobacco = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (smokerTobacco == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    /* Create semaphores */
    sem_create(agent, 0);
    sem_create(lock, 1);
    sem_create(smokerMatch, 0);
    sem_create(smokerPaper, 0);
    sem_create(smokerTobacco, 0);

    if ((pid = fork()) == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        // Child (agent process)
        printf("Agent's Pid: %d\n", getpid());
        N = attemptsLeft;
        flag1 = false;
        while (flag1 == false)
        {
            P(lock);
            fp1 = fopen("attempt.txt", "w+");
            fscanf(fp1, "%d", &attemptsLeft);
            printf("Agent has %d attempts left\n", attemptsLeft);

            int randNum = rand() % 3 + 1; // Pick a random number from 1-3
            if (randNum == 1)
            {
                // Put tobacco on table
                // Put paper on table
                printf("Agent put tobacco and paper on table\n");
                V(smokerMatch); // Wake up smoker with match
            }
            else if (randNum == 2)
            {
                // Put tobacco on table
                // Put match on table
                printf("Agent put tobacco and match on table\n");
                V(smokerPaper); // Wake up smoker with paper
            }
            else
            {
                // Put match on table
                // Put paper on table
                printf("Agent put match and paper on table\n");
                V(smokerTobacco); // Wake up smoker with tobacco
            }
            if (attemptsLeft == 1) {
                V(lock); // Allow smoker to go
                P(agent); // Agent sleeps until smoker finishes
            }

            /* Edit attempts file with new number of attempts left */
            fseek(fp1, 0L, 0);
            attemptsLeft--;
            fprintf(fp1, "%d\n", attemptsLeft);
            fclose(fp1);

            if (attemptsLeft == 0)
            {
                // printf("Agent will go %d more time\n", attemptsLeft);
                flag1 = true;
                V(lock); // Allow smokers to go
                V(smokerMatch);   // Wake up smoker with match to terminate itself
                V(smokerPaper);   // Wake up smoker with paper to terminate itself
                V(smokerTobacco); // Wake up smoker with tobacco to terminate itself
            }
            else
            {
                printf("Agent will go %d more time\n", attemptsLeft);
                V(lock); // Allow smoker to go
                printf("Agent sleep\n");
                P(agent); //  Agent sleeps until smoker finishes
            }
        }
    }

    else
    {
        // Parent
        if ((pid = fork()) == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            // Child (smoker with match)
            printf("Smoker with match's Pid: %d\n", getpid());
            flag2 = false;
            while (flag2 == false)
            {
                P(smokerMatch); // Sleep right away
                P(lock); // Only process out of the others running
                fp2 = fopen("attempt.txt", "r+");
                fscanf(fp2, "%d", &attemptsLeft);
                if (attemptsLeft == 0)
                {
                    fclose(fp2);
                    flag2 = true;
                    V(lock); // Allow other smokers to terminate
                }
                else
                {
                    // Pick up paper
                    // Pick up tobacco
                    printf("Smoker with match puts match on table\n");
                    V(agent); // Wake up agent upon completion
                    V(lock); // Allow agent to go
                    // Smoke (but don't inhale).
                }
            }
        }
        else
        {
            // parent
            if ((pid = fork()) == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid == 0)
            {
                // Child (smoker with paper)
                printf("Smoker with paper's Pid: %d\n", getpid());
                flag3 = false;
                while (flag3 == false)
                {
                    P(smokerPaper); // Sleep right away
                    P(lock); // Only process out of the others running
                    fp3 = fopen("attempt.txt", "r+");
                    fscanf(fp3, "%d", &attemptsLeft);
                    if (attemptsLeft == 0)
                    {
                        fclose(fp3);
                        flag3 = true;
                        V(lock); // Allow other smokers to terminate
                    }
                    else
                    {
                        // Pick up match
                        // Pick up tobacco
                        printf("Smoker with paper puts paper on table\n");
                        V(agent); // Wake up agent upon completion
                        V(lock); // Allow agent to go
                        // Smoke (but don't inhale).
                    }
                }
            }
            else
            {
                // Parent
                if ((pid = fork()) == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                if (pid == 0)
                {
                    // Child (smoker with tobacco)
                    printf("Smoker with tobacco's Pid: %d\n", getpid());
                    flag4 = false;
                    while (flag4 == false)
                    {
                        P(smokerTobacco); // Sleep right away
                        P(lock); // Only process out of the others running
                        fp4 = fopen("attempt.txt", "r+");
                        fscanf(fp4, "%d", &attemptsLeft);
                        if (attemptsLeft == 0)
                        {
                            fclose(fp4);
                            flag4 = true;
                            V(lock); // Allow other smokers to terminate
                        }
                        else
                        {
                            // Pick up match
                            // Pick up paper
                            printf("Smoker with tobacco puts tobacco on table\n");
                            V(agent); // Wake up agent upon completion
                            V(lock); // Allow agent to go
                            // Smoke (but don't inhale).
                        }
                    }
                }
                else
                {
                    // Parent

                    // Wait for the child processes to finish

                    pid = wait(&status);
                    printf("child(pid = %d) exited with the status %d. \n", pid, status);

                    pid = wait(&status);
                    printf("child(pid = %d) exited with the status %d. \n", pid, status);

                    pid = wait(&status);
                    printf("child(pid = %d) exited with the status %d. \n", pid, status);

                    pid = wait(&status);
                    printf("child(pid = %d) exited with the status %d. \n", pid, status);

                    /* Remove semaphores */
                    semkill(lock);
                    semkill(smokerMatch);
                    semkill(smokerPaper);
                    semkill(smokerTobacco);
                }
                exit(EXIT_SUCCESS);
            }
            exit(EXIT_SUCCESS);
        }
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_SUCCESS);
}