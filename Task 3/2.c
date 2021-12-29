#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t child = fork();
    if (child == 0) // child process
    {
        printf("Successful forking of child process, my PID is: %d\n", getpid());
        char *const params[] = {"ls", "-al", NULL};
        execvp("ls", &params[0]);
    }
    else if (child == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else // parent process
    {
        wait(NULL);
    }
    return 0;
}