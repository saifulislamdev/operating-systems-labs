#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    int pipefd[2];
    char *filepath = "readme.txt";

    if (pipe(pipefd) == -1) // error
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t cpid = fork();
    if (cpid == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) // child
    {
        int fd = open(filepath, O_RDONLY);
        if (-1 == fd) // error
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        /* Read from file */
        off_t totalBytes = lseek(fd, 0, SEEK_END); // get total bytes in the file by going to end of file
        if (totalBytes == -1)                      // error
        {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        if (lseek(fd, 0, SEEK_SET) == -1) // try to go back to beginning of file and catch error
        {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        char fileBuf[totalBytes];
        size_t count = read(fd, fileBuf, totalBytes); // read file contents
        if (count == -1)                              // error
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (count < totalBytes) // entire contents not read
        {
            printf("Entire contents of the file %s was not read\n", filepath);
            exit(EXIT_FAILURE);
        }

        /* Close file */
        int closeFd = close(fd);
        if (-1 == closeFd)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }

        /* Child writes to pipe */
        close(pipefd[0]);                      /* Close unused read end */
        write(pipefd[1], fileBuf, totalBytes); // write to pipe
    }
    else
    {
        /* Parent reads from pipe */
        close(pipefd[1]); /* Close unused write end */

        wait(NULL); /* Wait for child */
        int fd = open(filepath, O_WRONLY);
        if (-1 == fd) // error
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
        off_t totalBytes = lseek(fd, 0, SEEK_END); // go to end of file
        if (totalBytes == -1)                      // error
        {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        char buf[1];
        size_t count = write(fd, " Parent is writing:", 19);
        if (count == -1) // error
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        if (count < 19) // entire contents not written
        {
            printf("Text could not be written into the file\n");
            exit(EXIT_FAILURE);
        }
        while (read(pipefd[0], buf, 1) > 0) // read from pipe and write to file
        {
            write(fd, buf, 1);
        }
        close(pipefd[0]); // close pipe read end

        /* Close file */
        int closeFd = close(fd);
        if (-1 == closeFd)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
