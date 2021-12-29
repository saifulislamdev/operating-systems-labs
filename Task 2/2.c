#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    // parent P
    int a = 10, b = 25, fq = 0, fr = 0;
    fq = fork(); // fork a child - call it Process Q
    if (fq == 0)
    { // Child successfully forked
        a = a + b;
        printf("a = %d, b = %d, in process Q, process_id = %d\n", a, b, getpid()); // print values of a, b, and process_id
        fr = fork();                                                               // fork another child - call it Process R
        if (fr != 0)
        {
            b = b + 20;
            printf("a = %d, b = %d, in process Q and parent process of process R, process_id = %d\n", a, b, getpid()); // print values of a, b, and process_id
        }
        else
        {
            a = (a * b) + 30;
            printf("a = %d, b = %d, in process R, process_id = %d\n", a, b, getpid()); // print values of a, b and process_id
        }
    }
    else
    {
        b = a + b - 5;
        printf("a = %d, b = %d, in process P, process_id = %d\n", a, b, getpid()); // print values of a, b, and process_id
    }
    return 0;
}