#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    while (1)
    {
        /* Reading line from stdin */
        char *line = NULL;
        size_t lineLen = 0;
        getline(&line, &lineLen, stdin);

        /* Splitting input to get command and command arguments */
        char *args[2048];
        int currArg = 0;
        args[currArg] = strtok(line, " ");
        while (args[currArg] != NULL)
        {
            currArg++;
            args[currArg] = strtok(NULL, " ");
        }

        /* Adding null-terminated char */
        args[currArg - 1][strlen(args[currArg - 1]) - 1] = '\0';

        if (strcmp(args[0], "quit") == 0)
        {
            exit(EXIT_SUCCESS);
        }

        pid_t cpid = fork();
        if (cpid == 0) // Child process
        {
            execvp(args[0], args);
            exit(EXIT_SUCCESS);
        }
        else if (cpid == -1) // Error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        /* Wait for execution of command */
        wait(NULL);
    }

    return 0;
}