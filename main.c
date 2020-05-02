#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"

int main(int argc, char* argv[])
{

	char str[128];
	int policy, num;
	struct process *proc;

	scanf("%s", str);
	scanf("%d", &num);
	
	proc = (struct process *)malloc(num * sizeof(struct process));

	for (int i = 0; i < num; i++) {
		scanf("%s%d%d", proc[i].name,
			&proc[i].ready, &proc[i].exec);
	}

	if (str[0] == 'F') {
		policy = FIFO;
	}
	else if (str[0] == 'R') {
		policy = RR;
	}
	else if (str[0] == 'S') {
		policy = SJF;
	}
	else if (str[0] == 'P') {
		policy = PSJF;
	}

	scheduling(proc, num, policy);

	exit(0);
}
