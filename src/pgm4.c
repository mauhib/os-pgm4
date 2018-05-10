#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pgm4.h"

#define DEBUG 0
#define TEST 0

#define SERVICE_TIME_MIN 1
#define SERVICE_TIME_MAX 50

#define NUMBER_JOBS_PER_SIMULATION_TEST 5

#define NUMBER_JOBS_PER_SIMULATION 1000
#define NUM_SIMULATIONS 1000

#define NUM_ALGORITHMS 4
#define ALGO_FCFS 0
#define ALGO_RR 1
#define ALGO_SRT 2
#define ALGO_HRRN 3


//The program shall assume that the Jobs array is sorted
//in the order of monotonically increasing arrival times

void cleanJobArray(job_t* arr, int length){
	int i;
	for(i =0;i<length;i++){
		arr[i].start_time = 0;
		arr[i].complete_time = 0;
		arr[i].isComplete = 0;
	}
}

job_t* generateJobArray(int length){
	job_t* arr = calloc(sizeof(job_t),length);
	int i;
	for(i =0;i<length;i++){
		arr[i].arrival_time = i; //Each job becomes available at it's index
		arr[i].service_time = (rand() % SERVICE_TIME_MAX)+SERVICE_TIME_MIN;
	}
	return arr;
}

job_t* generateJobArrayTest(int length){
	job_t* arr = calloc(sizeof(job_t),length);
	arr[0].arrival_time = 0; arr[0].service_time = 3;
	arr[1].arrival_time = 2; arr[1].service_time = 6;
	arr[2].arrival_time = 4; arr[2].service_time = 4;
	arr[3].arrival_time = 6; arr[3].service_time = 5;
	arr[4].arrival_time = 8; arr[4].service_time = 2;
	return arr;
}

void printJobArray(job_t* jobs, int jobs_len){
	printf("|%5s|%7s|%7s|\n", "PID", "Arrival", "Service");
	int i;
	for(i =0;i<jobs_len;i++){
		printf("|%5d|%7d|%7d|\n", i, jobs[i].arrival_time, jobs[i].service_time);
	}
	printf("\n");
}

void cleanTimeArray(int* arr, int len){
	int i;
	for(i =0;i<len;i++){
		arr[i] = -1;
	}
}

int calculateSizeOfTimeArray(job_t* jobs, int jobs_len){
	int i;
	int size = 0;
	for(i =0;i<jobs_len;i++){
		size += jobs[i].service_time;
	}
	return size;
}

int* generateTimeArray(int len){
	int* arr = malloc(sizeof(int)*len);
	cleanTimeArray(arr,len);
	return arr;
}

//The following function is for the visualization of
//the scheduling algorithms. It should only be used for
//simulations with < 26 jobs
void printTimeArray(int* time, int time_len, int jobs_len){
	int i;
	int j;
	printf("   ");
	for(j=0; j<jobs_len;j++){
		printf("|%c",(char)(j+65));
	}
	printf("|\n");
	for(i=0;i<time_len;i++){
		if(time[i] == -1) break; //All done
		printf("%3d",i);
		for(j=0; j<jobs_len;j++){
			printf("|%c",(time[i]==j)?'X':' ');
		}
		printf("|\n");
	}
	printf("\n");
}

void FCFS(job_t* jobs, int jobs_len, int* time) {
		int i,j;
		for (i = 0; i < jobs_len; i++) {
				//Find start and completion times
				if (i == 0) {
						jobs[i].start_time = jobs[i].arrival_time;
						jobs[i].complete_time = jobs[i].arrival_time + jobs[i].service_time;
				} else {
						if (jobs[i].arrival_time > jobs[i-1].complete_time) {
								jobs[i].start_time = jobs[i].arrival_time;
								jobs[i].complete_time = jobs[i].arrival_time + jobs[i].service_time;
						} else {
								jobs[i].start_time = jobs[i-1].complete_time;
								jobs[i].complete_time = jobs[i-1].complete_time + jobs[i].service_time;
						}
				}

				//Schedule
				for(j = jobs[i].start_time; j < jobs[i].complete_time; j++){
						time[j] = i;
				}

		}
}

void printQueue(queue_t* queue){
	queue_item_t* cur;
	printf("Queue: [");
	for(cur=queue->first;cur;cur=cur->next){
		printf("%d, ",cur->id);
	}
	printf("]\n");
}

void AddToQueue(queue_t* queue, int data){
	//Add to End
	if(queue->first){
		queue_item_t* cur;
		for(cur=queue->first;cur->next;cur=cur->next);
		cur->next = (queue_item_t*)calloc(sizeof(queue_item_t),1);
		cur->next->id=data;
		cur->next->next=NULL;
	}
	else{
		queue->first = (queue_item_t*)calloc(sizeof(queue_item_t),1);
		queue->first->id = data;
		queue->first->next=NULL;
	}
}

void AddToFront(queue_t* queue, int data){
	//Add to Front
	queue_item_t* temp = (queue_item_t*)calloc(sizeof(queue_item_t),1);
	temp->id = data;
	temp->next = queue->first;
	queue->first = temp;
}

int RemoveFromQueue(queue_t* queue){
	//remove from first;
	if(queue->first){
		int data = (queue->first->id);
		queue_item_t* next = queue->first->next;
		free(queue->first);
		queue->first = next;
		return data;
	}
	else{
		return -1;
	}
}

void RR(job_t* jobs, int jobs_len, int* time) {
  int i,j;
	//Initializations
	queue_t *q1 = calloc(sizeof(queue_t),1);
	q1->first=NULL;

	for (i = 0; i < jobs_len; i++) {
		jobs[i].time_remaining = jobs[i].service_time;
	}

	j=0;//Current Time
	do{
		//Schedule
		int pid = RemoveFromQueue(q1);
		if(pid != -1){
			//printf("Time= %d; Removed %d\n",j,pid);
			//printQueue(q1);
			time[j++]=pid;
			jobs[pid].time_remaining--;
		}
		//Add jobs for current
		for (i = 0; i < jobs_len; i++) {
			if(jobs[i].arrival_time == j) {
				AddToQueue(q1,i);
				//printf("Time= %d; Added %d\n",j,i);
				//printQueue(q1);
			}
		}
		if(pid != -1){
			if(jobs[pid].time_remaining == 0){
				//Completed
				jobs[pid].complete_time = j;
			}
			else{
				AddToQueue(q1,pid);
				//printf("Time= %d; Added %d\n",j,pid);
				//printQueue(q1);
			}
		}
	}while(q1->first);
	free(q1);
}

void SRT(job_t* jobs, int jobs_len, int* time) { //Shortest Remaining Time
		int i,j;

		// finding completion times
		for (i = 0; i < jobs_len; i++) {
						jobs[i].time_remaining = jobs[i].service_time;
		}

		unsigned int scheduled = -1;
		for(j=0;; ){
				if(scheduled != -1 && jobs[scheduled].time_remaining > 0){
						for(i = 0; i < jobs_len; i++) {
								if(jobs[i].arrival_time == j && jobs[i].time_remaining < jobs[scheduled].time_remaining){
										scheduled = i;
								}
						}
						time[j] = scheduled;
						j++;
						jobs[scheduled].time_remaining--;
						if(jobs[scheduled].time_remaining == 0){
							//Completed
							jobs[scheduled].complete_time = j;
						}
						continue;
				}

				unsigned int shortest = -1;
				int shortestTime = SERVICE_TIME_MAX+1;
				for(i = 0; i < jobs_len; i++) {
						if(jobs[i].arrival_time <= j && jobs[i].time_remaining > 0){
								if(jobs[i].time_remaining < shortestTime){
										shortest = i;
										shortestTime = jobs[i].time_remaining;
								}
						}
				}

				if(shortest == -1) break; //No more;

				scheduled = shortest;
				time[j] = shortest;
				j++;
				jobs[scheduled].time_remaining--;
				if(jobs[scheduled].time_remaining == 0){
					//Completed
					jobs[scheduled].complete_time = j;
				}
		}
}

void HRRN(job_t* jobs, int jobs_len, int* time) {     //Highest Response Ratio Next
		unsigned int i,j;
		// finding completion times
		for (i = 0; i < jobs_len; i++) {
						jobs[i].isComplete = 0;
		}

		for (j=0; ; ){
				int highest = -1;
				double HRR = -1;
				for (i = 0; i < jobs_len; i++) {
						if(jobs[i].arrival_time <= j && jobs[i].isComplete == 0){
								double RR = ((double)((j-jobs[i].arrival_time) + jobs[i].service_time)/jobs[i].service_time);
								if(RR > HRR){
										highest = i;
										HRR = RR;
								}
						}
				}

				if(highest == -1) break; //All done

				for(i = j; i < (j+jobs[highest].service_time); i++){
						time[i] = highest;
				}
				j += jobs[highest].service_time;
				jobs[highest].isComplete = 1;
				//Completed
				jobs[highest].complete_time = j;
		}
}

void performCalculations(job_t* jobs, int jobs_len, float* turnaround, float* rel_turnaround){
	int i;
	float average_turnaround = 0;
	float average_rel_turnaround = 0;
	int count = 0;
	for(i=0;i<jobs_len;i++){
		jobs[i].turnaround = jobs[i].complete_time - jobs[i].arrival_time;
		jobs[i].rel_turnaround = ((float)(jobs[i].turnaround))/jobs[i].service_time;
		average_turnaround += jobs[i].turnaround;
		average_rel_turnaround += jobs[i].rel_turnaround;
		count++;
	}
	(*turnaround) = average_turnaround/count;
	(*rel_turnaround) = average_rel_turnaround/count;
}

int main(int argc, char **argv){
	//Setup Output file
	FILE *fp;
	if(argc > 1){
		printf("Ouptut file selected: %s\n",argv[1]);
		fp = fopen(argv[1], "w+");
		if(!fp){
			printf("Error: Cannot open file.\n");
			return 0;
		}
	}
	else{
		printf("No Ouput file given as command line argument.\nWriting to Stdout.\n\n");
		fp = stdout;
	}

	//Init
	int algo;
	int sim_num;
	srand(time(0));

	double sums[NUM_ALGORITHMS*2] = {0};

	#if DEBUG==0
	fprintf(fp,"|%4s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|%12s|\n", "Num"
		,"FCFS Tr","FCFS Tr/Ts","RR Tr","RR Tr/Ts"
		,"SRT Tr","SRT Tr/Ts","HRRN Tr","HRRN Tr/Ts");
	#endif

	for(sim_num=0;sim_num<NUM_SIMULATIONS;sim_num++){
		#if DEBUG==0
			fprintf(fp,"|%4d|",sim_num);
		#endif
		#if TEST
		int jobs_len = NUMBER_JOBS_PER_SIMULATION_TEST;
		job_t* jobs = generateJobArrayTest(jobs_len);
		#else
		int jobs_len = NUMBER_JOBS_PER_SIMULATION;
		job_t* jobs = generateJobArray(jobs_len);
		#endif
		#if DEBUG
		printJobArray(jobs,jobs_len);
		#endif
		int time_len = calculateSizeOfTimeArray(jobs, jobs_len);
		int* time = generateTimeArray(time_len);

		for(algo = 0; algo < NUM_ALGORITHMS;algo++){
			switch(algo){
				case ALGO_FCFS:{
					FCFS(jobs,jobs_len,time);
					#if DEBUG
					fprintf(fp,"FCFS\n");
					#endif
					break;
				}
				case ALGO_RR:{
					RR(jobs,jobs_len,time);
					#if DEBUG
					fprintf(fp,"RR\n");
					#endif
					break;
				}
				case ALGO_HRRN:{
					HRRN(jobs,jobs_len,time);
					#if DEBUG
					fprintf(fp,"HRRN\n");
					#endif
					break;
				}
				case ALGO_SRT:{
					SRT(jobs,jobs_len,time);
					#if DEBUG
					fprintf(fp,"SRT\n");
					#endif
					break;
				}
			}
			float turnaround = 0;
			float rel_turnaround = 0;
			performCalculations(jobs, jobs_len, &turnaround, &rel_turnaround);
			sums[2*algo]+=turnaround;
			sums[2*algo+1]+=rel_turnaround;
			#if DEBUG
			printTimeArray(time,time_len,jobs_len);
			fprintf(fp,"Average Turnaround Time = %f\n", turnaround);
			fprintf(fp,"Average Relative Turnaround Time = %f\n", rel_turnaround);
			#else
			fprintf(fp,"%12.2f|%12.2f|",turnaround,rel_turnaround);
			#endif
			cleanTimeArray(time,time_len);
			cleanJobArray(jobs,jobs_len);
		}
		#if DEBUG==0
		fprintf(fp,"\n");
		#endif
		free(time);
		free(jobs);
	}

	//Print Averages
	printf("|%6s|%24s|%24s|\n","","Turnaround Time", "Relative Turnaround Time");
	for(algo=0;algo<NUM_ALGORITHMS;algo++){
		switch(algo){
			case ALGO_FCFS:{
				printf("|%6s|","FCFS");
				break;
			}
			case ALGO_RR:{
				printf("|%6s|","RR");
				break;
			}
			case ALGO_HRRN:{
				printf("|%6s|","HRRN");
				break;
			}
			case ALGO_SRT:{
				printf("|%6s|","SRT");
				break;
			}
		}
		printf("%24.2f|%24.2f|\n",sums[2*algo]/NUM_SIMULATIONS,sums[2*algo+1]/NUM_SIMULATIONS);
	}

	//Cleanup File
	if(argc > 1) fclose(fp);
	return 0;
}
