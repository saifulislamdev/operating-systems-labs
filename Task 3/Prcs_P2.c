#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    char SOURCE_FILE[] = "source.txt";
    char DESTINATION_FILE_1[] = "destination1.txt";
    char DESTINATION_FILE_2[] = "destination2.txt";

    /* Open source file in read-only mode */
    int rFd = open(SOURCE_FILE, O_RDONLY);
    if (rFd == -1)
    {
        printf("Sorry, %s does not exist or can't be opened\n", SOURCE_FILE);
        perror("open");
        return 1;
    }

    /* Open first destination file in write-only mode */
    int wFd1 = open(DESTINATION_FILE_1, O_WRONLY);
    if (wFd1 == -1)
    {
        printf("Sorry, %s does not exist or can't be opened\n", DESTINATION_FILE_1);
        perror("open");
        return 1;
    }

    /* Open second destination file in write-only mode */
    int wFd2 = open(DESTINATION_FILE_2, O_WRONLY);
    if (wFd2 == -1)
    {
        printf("Sorry, %s does not exist or can't be opened\n", DESTINATION_FILE_2);
        perror("open");
        return 1;
    }

    char buf1[100]; // buffer for first destination file
    char buf2[50]; // bufffer for second destination file
    while (1)
    {
        /* Read next 100 characters from source file using buffer */
        size_t bytesRead1 = read(rFd, buf1, 100);
        if (bytesRead1 == -1)
        {
            perror("read");
            return 1;
        }

        /* Replace '1' with 'L' in buffer */
        for (size_t i = 0; i < bytesRead1; i++)
            if (buf1[i] == '1')
                buf1[i] = 'L';

        /* Write to first destination file from buffer */
        size_t bytesWritten1 = write(wFd1, buf1, bytesRead1);
        if (bytesWritten1 == -1)
        {
            perror("write");
            return 1;
        }

        /* End of file reached */
        if (bytesRead1 != 100)
            break;

        /* Read next 50 characters from source file using buffer */
        size_t bytesRead2 = read(rFd, buf2, 50);
        if (bytesRead2 == -1)
        {
            perror("read");
            return 1;
        }

        /* Replace '3' with 'E' in buffer */
        for (size_t i = 0; i < bytesRead2; i++)
            if (buf2[i] == '3')
                buf2[i] = 'E';

        /* Write to second destination file from buffer */
        size_t bytesWritten2 = write(wFd2, buf2, bytesRead2);
        if (bytesWritten2 == -1)
        {
            perror("write");
            return 1;
        }

        /* End of file reached */
        if (bytesRead2 != 50)
            break;
    }

    /* Close source file */
    int closeReadFd = close(rFd);
    if (closeReadFd == -1)
    {
        printf("Sorry, %s not closed successfully\n", SOURCE_FILE);
        perror("close");
        return 1;
    }

    /* Close first destination file */
    int closeWriteFd1 = close(wFd1);
    if (closeWriteFd1 == -1)
    {
        printf("Sorry, %s not closed successfully\n", DESTINATION_FILE_1);
        perror("close");
        return 1;
    }

    /* Close second destination file */
    int closeWriteFd2 = close(wFd2);
    if (closeWriteFd2 == -1)
    {
        printf("Sorry, %s not closed successfully\n", DESTINATION_FILE_2);
        perror("close");
        return 1;
    }
}