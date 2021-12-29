#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    /* Check if file is passed in as argument */
    if (2 != argc)
    {
        printf("\n Usage : \n");
        return 1;
    }

    /* Open file */
    char *filepath = argv[1];
    int fd = open(filepath, O_RDONLY);
    if (-1 == fd)
    {
        printf("\nSorry, %s does not exist or can't be opened\n", filepath);
        perror("open");
        return 1;
    }

    /* Read from file and print using buffer */
    char buf[1];
    while (1)
    {
        size_t count = read(fd, buf, 1);
        if (count == -1)
        {
            perror("read");
            return 1;
        }
        else if (count == 0)
        {
            break;
        }
        printf("%s", buf);
    }
    printf("\n");

    /* Close file */
    int closeFd = close(fd);
    if (-1 == closeFd)
    {
        printf("\nSorry, %s not closed successfully\n", filepath);
        perror("close");
        return 1;
    }
    return 0;
}
