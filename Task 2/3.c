#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Specify the end of series number for S3 as an input argument.\n");
        exit(EXIT_FAILURE);
    }
    char **endptr;
    int endOfSeries = strtol(argv[1], endptr, 0);
    pid_t child1 = fork();
    if (child1 == 0) // child1
    {
        int s2 = 0;
        for (size_t i = 1; i <= endOfSeries; i += 2)
        {
            s2 += i;
            printf("%ld added to S2, S2 = %d\n", i, s2);
        }
        printf("S2 = %d\n", s2);
        return 0;
    }
    else if (child1 == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else
    {
        int status1;
        waitpid(child1, &status1, 0); // wait for child 1
        pid_t child2 = fork();
        if (child2 == 0) // child 2
        {
            int s1 = 0;
            for (size_t i = 2; i <= endOfSeries; i += 2)
            {
                s1 += i;
                printf("%ld added to S1, S1 = %d\n", i, s1);
            }
            printf("S1 = %d\n", s1);
            return 0;
        }
        else if (child2 == -1) // error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else
        {
            int status2;
            waitpid(child2, &status2, 0); // wait for child 2
            int s3 = 0;
            for (size_t i = 1; i <= endOfSeries; i++)
            {
                s3 += i;
                printf("%ld added to S3, S3 = %d\n", i, s3);
            }
            printf("S3 = %d\n", s3);
        }
    }
    return 0;
}