#ifndef _PGM4_H
#define _PGM4_H

struct job{
	int arrival_time;
	int service_time;
	int start_time; //FCFS
	int complete_time; //FCFS
	int isComplete; //HRRN
	int time_remaining; //SRT, RR
	int turnaround;
	float rel_turnaround;
} typedef job_t;

typedef struct queue_item{
	int id;
	struct queue_item* next;
} queue_item_t;

typedef struct queue{
	queue_item_t* first;
} queue_t;

#endif /* _PGM4_H */
