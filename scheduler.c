#define _GNU_SOURCE
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>

/* For RR */
int cirque[30], front = -1, rear = -1;

int isEmpty(){
	if(front == -1) return 1;
	return 0;
}
void enQueue(int item){
	if(front == -1) front = 0;
	rear = (rear+1)%30;
	cirque[rear] = item;
}
int deQueue(){
	int item;
	item = cirque[front];
	if(front == rear){
		front = -1;
		rear  = -1;
	}
	else{
		front = (front+1)%30;
	}
	return (item);
}


/* For RR */
int last;

int now = 0;

/* who is running */
int running = -1;

/* number of finished processes */
int count = 0;

/* Maybe not necessarily needed sorted  */
int cmp(const void *a, const void *b) {
	return ((struct process *)a)->ready - ((struct process *)b)->ready;
}


int SELECT(struct process *proc, int num, int policy)
{
	
	if (running != -1 && (policy == SJF || policy == FIFO))
		return running;
	
	int ret = -1;

	if (policy == PSJF || policy ==  SJF) {
		for (int i = 0; i < num; i++) {
			if (proc[i].pid == -1 || proc[i].exec == 0)
				continue;
			/* O(n) find min */
			if (ret == -1 || proc[i].exec < proc[ret].exec)
				ret = i;
		}
	}

	else if (policy == FIFO) {
		for(int i = 0; i < num; i++) {
			if(proc[i].pid == -1 || proc[i].exec == 0)
				continue;
			/* O(n) find min */
			if(ret == -1 || proc[i].ready < proc[ret].ready)
				ret = i;
		}
        }

	else if (policy == RR) {
		if (running == -1) {
			if(!isEmpty()){
				ret = deQueue();
			}
		}
		else if ((now - last) % 500 == 0)  {
			enQueue(running);
			ret = deQueue();
		}
		else
			ret = running;
	}

	return ret;
}

int scheduling(struct process *proc, int num, int policy)
{
	int flag[num];
	long t_start[num];
	long t_end[num];
	static const long BASE = 1000000000;

	qsort(proc, num, sizeof(struct process), cmp);

	/* initialize ready queue and time_start*/
	for (int i = 0; i < num; i++){
		proc[i].pid = -1;
		flag[i] = 1;
	}


	/* initialize scheduler */
	allocate(getpid(), 0);
	increase(getpid());
	
	while(1) {
	

		/* record the finish time of child process */
		if (running != -1 && proc[running].exec == 0) {
		
			waitpid(proc[running].pid, NULL, 0);
			printf("%s %d\n", proc[running].name, proc[running].pid);
			fflush(stdout);
			t_end[running] = syscall(335);
			syscall(334, proc[running].pid, t_start[running], t_end[running]);

			running = -1;
			count++;

			if (count == num)
				break;
		}

		/* create ready process */
		for (int i = 0; i < num; i++) {
			if (proc[i].ready == now) {
				enQueue(i);
				proc[i].pid = RUN(proc[i]);
				decrease(proc[i].pid);
			}

		}

		int next = SELECT(proc, num, policy);
		if (next != -1) {
			/* Context switch */
			if (running != next) {

				/* time_start */
				if(flag[next] == 1){
					 t_start[next] = syscall(335);
					 flag[next] = 0;
				}

				increase(proc[next].pid);
				decrease(proc[running].pid);
				running = next;
				last = now;
			
			}
		}

		volatile unsigned long gg;
		for(gg = 0; gg < 1000000UL; gg++);

		if (running != -1)
			proc[running].exec--;
		now++;
	}

	/* time measurement 
	long sum = 0;
	for(int i = 0; i < num; i++){
		sum += t_end[i] - t_start[i];
	}
	sum /= 5000;
	printf("avg unit = %ld.%09ld\n", sum/BASE, sum % BASE); */

	return 0;
}
