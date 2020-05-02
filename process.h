#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>


struct process {
	pid_t pid;
	char name[32];
	int ready;
	int exec;
};

void allocate(int pid, int core);

int RUN(struct process proc);

int decrease(int pid);

int increase(int pid);

#endif
