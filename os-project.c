#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MIN_PID 100
#define MAX_PID 1000
#define MAX_THREADS 100
#define MAX_SLEEP_SEC 15

int *process_id;
int pid_min,pid_max,no_of_threads,size;

int allocate_map();
int allocate_pid();
void release_pid(int pid);


int allocate_map()
 {
	size = (pid_max - pid_min + 1);
	process_id =(int*)calloc(size, sizeof(int));

	if (process_id != NULL)
		return 1;
    else
	return -1;
}


int allocate_pid()
 {
	int x = 0;
	while (process_id[x] != 0) 
	{
		x++;
	}
	if (x + pid_min > pid_max)
	{
		return -1;
	}
	process_id[x] = 1;

	return(x + pid_min);
}


void release_pid(int pid) 
{
	if (pid < pid_min || pid > pid_max) 
	{
		printf("\n!!!ERROR: Invalid PID. It must be inside the interval {%d - %d}\n", pid_min, pid_max);
		return;
	}
	process_id[pid - pid_min] = 0;
}

pthread_mutex_t lock;
void *threading(int argument) 
{
    srand(time(0));
    int sleep_time = rand() % MAX_SLEEP_SEC;	
    int pid = -1;
    while(pid == -1) 
	{
        pthread_mutex_lock(&lock);
        pid = allocate_pid();
        if (pid == -1) 
	    {
        printf("\n\n!!! Can't allocate PID because of full usage of PID !!!\t THREAD : %d has to wait for pid to release.\n",argument+1);
		sleep(1);
	    }
        pthread_mutex_unlock(&lock);
    }

    printf("\n Thread: %d has taken PID: %d .\t PROCESSING for %d seconds\n",argument+1, pid, sleep_time);
    sleep(sleep_time);
    printf("\n Thread: %d has COMPLETED. \t PID: %d  has been RELEASED.\n",argument+1, pid);
    release_pid(pid);

    return NULL;
}

int main()
 {
    int var;
    printf("\n\n\t\tWELCOME TO PROCESS ID MANAGER \n\n");
    printf("\nEnter Minimum and Maximum Value of PID (In between 100-1000) :-\n\n");
    printf("Min PID value : ");
    scanf("%d",&pid_min);

    printf("Max PID value : ");
    scanf("%d",&pid_max);

    printf("\n\nEnter number of thread(s) you want to create (MAX :100) : ");
    scanf("%d",&no_of_threads);

    if(pid_min < MIN_PID || pid_max > MAX_PID || no_of_threads > MAX_THREADS)
	{
	printf("\n You have entered wrong values !!!\t Program is terminating.\n");
	return 1;
	}

    printf("\n\n========================|  EXECUTING  |==========================\n\n");
    
	pthread_t my_thread[no_of_threads];
    pthread_mutex_init(&lock, NULL);
    if (allocate_map() == -1) 
	{
        printf("!!!ERROR: Could not initialize the PID. Terminating program.\n");
        return -1;
    }

    for (var = 0; var < no_of_threads; var++) 
	{
        pthread_create(&my_thread[var],NULL,threading,var);
	    sleep(1);
    }

    for (var = 0; var < no_of_threads; var++)	
	{
        pthread_join(my_thread[var],NULL);	
    }

    if (process_id != NULL) 
	{
        free(process_id);
    }

    printf("\n\n \t\tALL THE THREADS HAVE DONE THEIR EXECUTION \n\n");
	printf("=========================| THANK YOU |============================\n\n\n");
    return 0;
}
