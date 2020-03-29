#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MIN_PID 100               //declaring minimum value of PID as 100 which is a constant
#define MAX_PID 1000              //declaring maximum value of PID as 1000
#define MAX_THREADS 100          //declaring maximum number of threads as 100
#define MAX_SLEEP_SEC 15         //declaring maximum sllep time for a thread as 15

int *process_id;                 //pointer variable for dynamic memory allocation
int pid_min,pid_max,no_of_threads,size;

int allocate_map();                //function that creates and initialises data structure for representing PIDs 
int allocate_pid();               //function that allocates and returns a pid; returns- 1 if unable to allocate a pid
void release_pid(int pid);        // this function releases a pid

int allocate_map()
 {
	size = (pid_max - pid_min + 1);                //range of PIDs is calculated
	process_id =(int*)calloc(size, sizeof(int));   //intialisation of pointer variable using calloc
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
        printf("\n!!!Error: Invalid pid. It must lie between %d - %d \n", pid_min, pid_max);                                                                                    
	    return;
	}
	process_id[pid - pid_min] = 0;
}

pthread_mutex_t lock;             //use of mutex lock mechanism
void *threading(int argument)    //function that allocates pids to threads
{
    srand(time(0));               //use of srand function that generates a random number each time
    int sleep_time = rand() % MAX_SLEEP_SEC;	//this will generate the time for sleeping for                           //thread from any integer between 0 and 15
    int pid = -1;
    while(pid == -1) 
	{
        pthread_mutex_lock(&lock);            //here locks are initiated for a thread
        pid = allocate_pid();                 //allocation of PID
        if (pid == -1) 
	    {
        printf("\n\n!!! Can't allocate PID because of full usage of PID !!!\t THREAD : %d needs to wait for PID to release.\n",argument+1);
		sleep(1);
	    }
        pthread_mutex_unlock(&lock);          //unlocking 
    }

    printf("\n Thread: %d has taken PID: %d .\t PROCESSING for %d seconds\n",argument+1, pid, sleep_time);
    sleep(sleep_time);                     //a thread sleeps for a random time
    printf("\n Thread: %d has COMPLETED. \t PID: %d  has been RELEASED.\n",argument+1, pid);
    release_pid(pid);                      //release of PID

    return NULL;
}

int main()
 {
    int var;
    printf("\n\n\t\tWELCOME TO PROCESS ID MANAGER \n\n");
    printf("\nEnter Boundary values for PID (In between 100-1000) :-\n\n");
    printf("Min PID value : ");
    scanf("%d",&pid_min);

    printf("Max PID value : ");
    scanf("%d",&pid_max);

    printf("\n\nHow many thread(s) you wish to create (MAX :100) : ");
    scanf("%d",&no_of_threads);

    if(pid_min < MIN_PID || pid_max > MAX_PID || no_of_threads > MAX_THREADS || no_of_threads < 0)
	{
	printf("\n You have entered wrong values !!!\t Program is terminating.\n");
	return 1;
	}

    printf("\n\n========================|  EXECUTING  |==========================\n\n");
    
	pthread_t my_thread[no_of_threads];             //declaration of thread(s)
    pthread_mutex_init(&lock, NULL);
    if (allocate_map() == -1) 
	{
        printf("!!!ERROR: Could not initialize the PID. Terminating program.\n");
        return -1;
    }

    for (var = 0; var < no_of_threads; var++) 
	{
        pthread_create(&my_thread[var],NULL,threading,var);    //creation of a thread
	    sleep(1);
    }

    for (var = 0; var < no_of_threads; var++)	
	{
        pthread_join(my_thread[var],NULL);	        //joining of thread with the main process
    }

    if (process_id != NULL) 
	{
        free(process_id);             // pointer variable is freed
    }

    printf("\n\n \t\tALL THREADS - FINALLY EXECUTED !!!\n\n");
	printf("=========================| THANK YOU |============================\n\n\n");
    return 0;                      
}

