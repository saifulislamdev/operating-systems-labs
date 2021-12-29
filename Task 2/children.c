#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t child1 = fork();
    if (child1 == 0) // child process
    {
        printf("I am child one, my pid is: %d\n", getpid());
        return 0;
    }
    else if (child1 == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else // parent process
    {
        pid_t child2 = fork();
        if (child2 == 0) // child
        {
            printf("I am child two, my pid is: %d\n", getpid());
            return 0;
        }
        else if (child2 == -1) // error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else // parent
        {
            int status1;
            waitpid(child1, &status1, 0);
            int status2;
            waitpid(child2, &status2, 0);
        }
    }
    return 0;
}