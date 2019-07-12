/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 9th, 2019
 *****************************************************************************/
#include <proc.h>

/* Globals */
/* Array of processes for the scheduler. */
struct pcb ptable[MAX_PROC];
/* Pid of the current process. */
int currpid = 0;
/* From sched.c */
extern int maxpid;

/* Set all procs to unused state. */
void init_ptable() {
	int i;
	for(i = 0; i < MAX_PROC; i++) {
		ptable[i].state = UNUSED;
	}
}
/* Return the process that is currently RUNNING. */
struct pcb currproc() {
	return ptable[currpid];
}

void scheduler() {
/* Current index of the scheduler. */
	static int index = 0;
	while(1) {
/* Reset if we'er looking passed the largest pid, there will be no RUNNABLE */
/* processes passed that index. */
		if(index > maxpid || index > MAX_PROC) {
			index = 0;
		}
		else if(ptable[index].state == RUNNABLE) {
			currpid = ptable[index].pid;
		}
		else {
			index++;
		}
	}
}	
