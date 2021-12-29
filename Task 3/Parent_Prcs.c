#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t child1 = fork();
    if (child1 == 0) // child 1
    {
        char *const params1[] = {"./Prcs_P1", NULL};
        execv("./Prcs_P1", &params1[0]);
        return 0;
    }
    else if (child1 == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else // parent process of child 1
    {
        int status1;
        waitpid(child1, &status1, 0);
        pid_t child2 = fork();
        if (child2 == 0) // child 2
        {
            char *const params2[] = {"./Prcs_P2", NULL};
            execv("./Prcs_P2", &params2[0]);
        }
        else if (child2 == -1) // error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else // parent process of child 1 and 2
        {
            int status2;
            waitpid(child2, &status2, 0);
            return 0;
        }
    }
    return 0;
}