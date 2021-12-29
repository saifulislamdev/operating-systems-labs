#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    int students = 10;

    char *filepath = "quiz_grades.txt";
    int fd = open(filepath, O_RDONLY);
    if (-1 == fd) // error
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* Get total bytes in the file by going to end of file */
    off_t totalBytes = lseek(fd, 0, SEEK_END);
    if (totalBytes == -1) // error
    {
        perror("lseek");
        exit(EXIT_FAILURE);
    }

    /* Go back to beginning of file */
    if (lseek(fd, 0, SEEK_SET) == -1) // error
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

    if (close(fd) == -1) // error
    {
        perror("close");
        exit(EXIT_FAILURE);
    }

    /* Count number of homeworks given to each student */
    size_t i = 0;
    size_t totalNumOfHWs = 1;
    while (fileBuf[i] != '\n')
        if (fileBuf[i++] == ' ')
            totalNumOfHWs++;

    /* Get number of chapters */
    size_t x;
    for (x = 2; x <= totalNumOfHWs; x++)
        if (totalNumOfHWs % x == 0)
            break;

    /* Number of HWs in each chapter */
    size_t y = totalNumOfHWs / x;

    /* Get grades from file */
    FILE *file = fopen(filepath, "r");
    int grades[students][totalNumOfHWs], grade, newLine;
    size_t currStudent = 0, currHW = 0;
    char line[totalBytes];
    char *row = fgets(line, sizeof(line), file);

    while (row != NULL) // get all rows
    {
        while (sscanf(row, "%d%n", &grade, &newLine) == 1) // get grades in row
        {
            grades[currStudent][currHW] = grade;
            row += newLine;
            currHW++;
        }
        if (errno != 0)
            perror("scanf");
        currHW = 0;
        currStudent++;
        row = fgets(line, sizeof(line), file);
    }

    fclose(file);

    for (size_t i = 0; i < x; i++) // Manager processes
    {
        pid_t manager = fork();

        if (manager == 0)
        {
            for (size_t j = 0; j < y; j++) // Worker processes
            {

                pid_t worker = fork();
                if (worker == 0)
                {
                    float sum = 0;
                    for (size_t k = 0; k < students; k++) // get sum of grades for a HW in a certain chapter
                        sum += grades[k][(i * y) + j];

                    float average = sum / students;
                    printf("Chapter %lu Homework %lu Average: %f\n", i + 1, j + 1, average);
                    break;
                }
                else if (worker == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else
                    wait(NULL);
            }
            break;
        }
        else if (manager == -1) // error
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else
            wait(NULL);
    }

    return 0;
}