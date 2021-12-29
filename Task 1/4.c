#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char sourceFile[] = "source.txt";
    char destinationFile[] = "destination.txt";

    /* Open source file in read-only mode */
    int rFd = open(sourceFile, O_RDONLY);
    if (-1 == rFd)
    {
        printf("\nSorry, %s does not exist or can't be opened\n", sourceFile);
        perror("open");
        return 1;
    }

    /* Open destination file in write-only mode */
    int wFd = open(destinationFile, O_WRONLY);
    if (-1 == wFd)
    {
        printf("\nSorry, %s does not exist or can't be opened\n", destinationFile);
        perror("open");
        return 1;
    }

    char buf[100];
    while (1)
    {
        /* Read next 100 characters from source file using buffer */
        size_t bytesRead = read(rFd, buf, 100);
        if (bytesRead == -1)
        {
            perror("read");
            return 1;
        }
        
        /* Replace '1' with 'L' in buffer */
        for (size_t i = 0; i < bytesRead; i++)
            if (buf[i] == '1')
                buf[i] = 'L';

        /* Write to destination file from buffer */
        size_t bytesWritten = write(wFd, buf, bytesRead);
        if (bytesWritten == -1)
        {
            perror("write");
            return 1;
        }
        if (bytesRead != 0)
        {
            size_t bytesWrittenXYZ = write(wFd, "XYZ", 3);
            if (bytesWrittenXYZ == -1)
            {
                perror("write");
                return 1;
            }
        }
        if (bytesRead != 100)
            break;
    }
    printf("Write to %s from %s succeeded\n", destinationFile, sourceFile);

    /* Close source file */
    int closeReadFd = close(rFd);
    if (-1 == closeReadFd)
    {
        printf("\nSorry, %s not closed successfully\n", sourceFile);
        perror("close");
        return 1;
    }

    /* Close destination file */
    int closeWriteFd = close(wFd);
    if (-1 == closeWriteFd)
    {
        printf("\nSorry, %s not closed successfully\n", destinationFile);
        perror("close");
        return 1;
    }

    return 0;
}
