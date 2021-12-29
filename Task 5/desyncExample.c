#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "sem.h"

#define N 10
#define Page 2
#define ProduceCapacity 7



FILE *fp1, *fp2;

int main(int argc, char* argv[]) {

    srand(time(NULL));
    int status;


    fp1 = fopen("printerPapers.txt", "w+");
    fprintf(fp1, "%d\n", 0);
	fclose(fp1);


    pid_t pid;

    //Create child processes that will do the updates
	if ((pid = fork()) == -1) 
	{
		//fork failed!
		perror("fork");
		exit(1);
	}
    else if(pid == 0){
        printf("in producer: %d\n",getpid());
        int i=N;
        while (i>0) {
            // Produce
           
            // Add pages to the printer
            int x;
            fp1 = fopen("printerPapers.txt", "r+");
            fscanf(fp1, "%d", &x);
            printf("producer sees number of pages available: %d\n",x);
            fseek(fp1,0L,0);
            fprintf(fp1, "%d\n", x+Page);
            i--;
            printf("Producer: after production, Paper count: %d\n",x+Page);
            fclose(fp1);
            /*
            int r = rand()%5;
            printf("Producer will wait for %d seconds\n",r);
            sleep(r);
            */

        }

    }
	
    else {

        printf("consumer pid: %d\n",getpid());
        //consumer
        //sleep(2);
        int i=N;
        while (i>0) {
            int y;
            
            // Consume pages from printe
            int x;
            fp2 = fopen("printerPapers.txt", "r+");
            fscanf(fp2, "%d", &x);
            printf("Consumer sees number of pages available: %d ",x);
            fseek(fp2,0L,0);
            fprintf(fp2, "%d\n", x-Page);
            i--;
            printf("Consumer: after consumption, Paper count: %d\n",x-Page);  
            fclose(fp2);
            /*
            int r = rand()%5;
            printf("Consumer will wait for %d seconds\n",r);
            sleep(r);
            */
            
        }

    }

    return 0;
}