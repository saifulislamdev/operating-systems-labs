#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sem.h"

#define CHILD 0 /* Return value of child process from fork call */
#define TRUE 0
#define FALSE 1

#define NumOfDepositAttempt 5	/* How many times Dad will deposit money */
#define NumOfWithdrawAttempt 20 /* Total number of attempts SON_1 and SON_2 has */

#define DepositAmount 60  /* The amound of money Dad deposits at a time */
#define WithdrawAmount 15 /* The amount of money Son withdraws at a time */
#define initBalance 0

FILE *fp1, *fp2, *fp3, *fp4; /* File Pointers */

int main()
{
	int pid;		 // Process ID after fork call
	int i;			 // Loop index
	int N;			 // Number of times dad does update
	int N_Att;		 // Number of time sons allowed to do update
	int status;		 // Exit status of child process
	int bal1, bal2;	 // Balance read by processes
	int flag, flag1; // End of loop variables
	srand(time(NULL));

	int balance = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (balance == -1)
	{
		perror("semget");
		exit(1);
	}
	int parentOrSon = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (parentOrSon == -1)
	{
		perror("semget");
		exit(1);
	}

	int oneSonAtATime = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (oneSonAtATime == -1)
	{
		perror("semget");
		exit(1);
	}

	// don't allow for negative balances
	sem_create(balance, initBalance);

	// either son or parent any given time so that balances match up
	sem_create(parentOrSon, 1);

	// only one son can go at a time
	// 0 means a son is currently at the ATM, 1 means no son currently at the ATM
	sem_create(oneSonAtATime, 1);

	//Initialize the file balance to be $100
	fp1 = fopen("balance.txt", "w+");
	bal1 = initBalance;
	fprintf(fp1, "%d\n", bal1);
	fclose(fp1);

	//Initialize the number of withdraw attempts
	fp4 = fopen("attempt.txt", "w+");
	N_Att = NumOfWithdrawAttempt;
	fprintf(fp4, "%d\n", N_Att);
	fclose(fp4);

	//Create child processes that will do the updates
	if ((pid = fork()) == -1)
	{
		// fork failed!
		perror("fork");
		exit(1);
	}

	if (pid == CHILD)
	{
		// First Child Process. Dad tries to do some updates.
		printf("Dad's Pid: %d\n", getpid());
		N = NumOfDepositAttempt;
		for (i = 1; i <= N; i++)
		{
			printf("Dad is requesting to view the balance.\n"); //Dad is requesting to get hold of an ATM.
			P(parentOrSon);
			fp1 = fopen("balance.txt", "r+"); //Dad successfully got hold of the ATM.
			fscanf(fp1, "%d", &bal2);
			printf("Dad reads balance = %d \n", bal2);
			int r = rand() % 5 + 1;
			printf("Dad needs %d sec to prepare money\n", r);
			sleep(r); //Dad Process is sleeping for r sec. You need to make sure that other processes can work in the mean time.

			fseek(fp1, 0L, 0); //Dad will now deposit the money. For this Dad will access the ATM again. And update the current balance.
			bal2 += DepositAmount;
			fprintf(fp1, "%d \n", bal2);
			fclose(fp1);
			for (size_t d = 0; d < DepositAmount; d++)
				V(balance);
			printf("Dad writes new balance = %d \n", bal2);
			printf("Dad will deposit %d more time\n", N - i); //Dad depostited the money.
			V(parentOrSon);
			sleep(rand() % 10 + 1); /* Dad will wait some time for requesting to see balance again.*/
		}
	}

	else
	{

		//Parent Process. Fork off another child process.
		if ((pid = fork()) == -1)
		{
			//Fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == CHILD)
		{
			printf("First Son's Pid: %d\n", getpid());
			// Second child process. First poor son tries to do updates.
			flag = FALSE;
			while (flag == FALSE)
			{
				P(oneSonAtATime);									  // block other son from performing withdrawals or wait for other son to finish performing withdrawal
				printf("SON_1 is requesting to view the balance.\n"); //Son_1 is requesting to get hold of the ATM.
				fp3 = fopen("attempt.txt", "r+");					  //Son_1 successfully got hold of the ATM.
				fscanf(fp3, "%d", &N_Att);							  // Son_1 Checks if he has more than 0 attempt remaining.
				printf("Attempt remaining: %d.\n", N_Att);
				if (N_Att == 0)
				{
					fclose(fp3);
					flag = TRUE;
				}
				else
				{
					for (size_t w = 0; w < WithdrawAmount; w++)
						P(balance);
					P(parentOrSon);					  // block here so parent does not deposit in the middle of withdrawal and child does not withdraw in the middle of deposit/withdrawal
					fp2 = fopen("balance.txt", "r+"); //Son_1 reads the balance.
					fscanf(fp2, "%d", &bal2);
					printf("SON_1 reads balance. Available Balance: %d \n", bal2);
					printf("SON_1 wants to withdraw money. "); //And if balance is greater than Withdraw amount, then son can withdraw money.
					fseek(fp2, 0L, 0);
					bal2 -= WithdrawAmount;
					fprintf(fp2, "%d\n", bal2);
					fclose(fp2);
					V(parentOrSon);
					printf("SON_1 withdrawed %d. New Balance: %d \n", WithdrawAmount, bal2);

					fseek(fp3, 0L, 0); //SON_1 will write the number of  attempt remaining in the attampt.txt file.
					N_Att -= 1;
					fprintf(fp3, "%d\n", N_Att);
					fclose(fp3);
					printf("Number of attempts remaining:%d \n", N_Att);
				}
				V(oneSonAtATime);		// allow other son to perform withdrawal
				sleep(rand() % 10 + 1); //SON_1 will wait some time before the next request.
			}
		}
		else
		{

			// Parent Process. Fork off one more child process.
			if ((pid = fork()) == -1)
			{
				// fork failed !
				perror("fork");
				exit(1);
			}
			if (pid == CHILD)
			{
				printf("Second Son's Pid: %d\n", getpid());
				// Third child process.Second poor son tries to do updates.flag1 = FALSE;
				while (flag1 == FALSE)
				{
					P(oneSonAtATime);									  // block other son from performing withdrawals or wait for other son to finish performing withdrawal
					printf("SON_2 is requesting to view the balance.\n"); //Son_2 is requesting to get hold of the ATM.
					fp3 = fopen("attempt.txt", "r+");					  //Son_2 successfully got hold of the ATM.
					fscanf(fp3, "%d", &N_Att);							  // Son_2 Checks if he has more than 0 attempt remaining.
					printf("Attempt remaining: %d.\n", N_Att);
					if (N_Att == 0)
					{
						fclose(fp3);
						flag1 = TRUE;
					}
					else
					{
						for (size_t w = 0; w < WithdrawAmount; w++)
							P(balance);
						P(parentOrSon);					  // block here so parent does not deposit in the middle of withdrawal and child does not withdraw in the middle of deposit/withdrawal
						fp2 = fopen("balance.txt", "r+"); //Son_2 reads the balance.
						fscanf(fp2, "%d", &bal2);
						printf("SON_2 reads balance. Available Balance: %d \n", bal2);
						printf("SON_2 wants to withdraw money. "); //And if balance is greater than Withdraw amount, then son can withdraw money.
						fseek(fp2, 0L, 0);
						bal2 -= WithdrawAmount;
						fprintf(fp2, "%d\n", bal2);
						fclose(fp2);
						V(parentOrSon);
						printf("SON_2 withdrawed %d. New Balance: %d \n", WithdrawAmount, bal2);

						fseek(fp3, 0L, 0); //SON_2 will write the number of  attempt remaining in the attampt.txt file.
						N_Att -= 1;
						fprintf(fp3, "%d\n", N_Att);
						fclose(fp3);
						printf("Number of attempts remaining: %d \n", N_Att);
					}
					V(oneSonAtATime);		// allow other son to perform withdrawal
					sleep(rand() % 10 + 1); //SON_2 will wait some time before the next request.
				}
			}
			else
			{

				//Now parent process waits for the child processes to finish

				pid = wait(&status);
				printf("child(pid = %d) exited with the status %d. \n", pid, status);

				pid = wait(&status);
				printf("child(pid = %d) exited with the status %d. \n", pid, status);

				pid = wait(&status);
				printf("child(pid = %d) exited with the status %d. \n", pid, status);

				semkill(balance);
				semkill(parentOrSon);
				semkill(oneSonAtATime);
			}
			exit(0);
		}
		exit(0);
	}
	exit(0);
}