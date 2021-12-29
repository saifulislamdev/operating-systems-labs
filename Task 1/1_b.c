#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char filepath[] = "destination.txt";

    // Open file in read-only mode
    int fd = open(filepath, O_RDONLY | O_CREAT, S_IRWXU);
    if (-1 != fd)
        printf("\n%s opened in read-only or created then opened in read-only\n", filepath);
    else
    {
        printf("\nSorry, I couldn't open or create then open %s\n", filepath);
        perror("open");
        return 1;
    }

    // Close file
    int closeFd = close(fd);
    if (-1 != closeFd)
        printf("\n%s closed successfully\n", filepath);
    else
    {
        printf("\nSorry, %s not closed successfully\n", filepath);
        perror("close");
        return 1;
    }
    return 0;
}
