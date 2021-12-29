#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    char FILE1[] = "destination1.txt";
    char FILE2[] = "destination2.txt";

    /* Create FILE1 */
    int fd1 = open(FILE1, O_RDONLY | O_CREAT, S_IRWXU);
    if (fd1 == -1)
    {
        printf("Sorry, I couldn't create then open %s\n", FILE1);
        perror("open");
        return 1;
    }

    /* Create FILE2 */
    int fd2 = open(FILE2, O_RDONLY | O_CREAT, S_IRWXU);
    if (fd2 == -1)
    {
        printf("Sorry, I couldn't create then open %s\n", FILE2);
        perror("open");
        return 1;
    }

    /* Close FILE1 */
    int closeFd1 = close(fd1);
    if (closeFd1 == -1)
    {
        printf("Sorry, %s not closed successfully\n", FILE1);
        perror("close");
        return 1;
    }

    /* Close FILE2 */
    int closeFd2 = close(fd2);
    if (closeFd2 == -1)
    {
        printf("Sorry, %s not closed successfully\n", FILE2);
        perror("close");
        return 1;
    }
    return 0;
}