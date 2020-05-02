#define _GNU_SOURCE
#include <stdio.h>
#include "process.h"
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>



void allocate(int pid, int core)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	sched_setaffinity(pid, sizeof(mask), &mask);
}

int RUN(struct process proc)
{
	int pid = fork();


	if (pid == 0)
	{
		for (int i = 0; i < proc.exec; i++)
		{
			volatile unsigned long j;
			for(j = 0 ;j < 1000000UL; j++);
		}
		exit(0);
	}
	allocate(pid, 1);
	return pid;
}

int decrease(int pid)
{
	struct sched_param param;
	param.sched_priority = 0;
	int res = sched_setscheduler(pid, SCHED_IDLE, &param);
	return res;
}

int increase(int pid)
{
	struct sched_param param;
	param.sched_priority = 0;
	int res = sched_setscheduler(pid, SCHED_OTHER, &param);
	return res;
}
