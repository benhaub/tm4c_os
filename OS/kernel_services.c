/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.c
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <proc.h>
#include <types.h>
#include <cstring.h>
#include <mem.h>

/* From proc.c */
extern int maxpid;
extern struct pcb ptable[];
/* From mem.c */
extern int ispid;

/*
 * Creates a new process. The forker forks the forked. Forker returns the pid
 * of the new process, forked returns NULLPID. Returns -1 on failure.
 */
int sysfork() {
	struct pcb *forked = reserveproc(NULL);
	if(NULL == forked) {
		return -1;
	}
	struct pcb *forker = currproc();
	forker->numchildren++;
/* Copy some info from the process that forked to the forked process. */
	forked->context.pc = forker->context.pc;
	forked->ppid = forker->pid;
/* Forked will return NULLPID to the user process. */
	forked->context.r0 = NULLPID;
	return forked->pid;
}

/*
 * The calling process waits for the process belonging to pid to exit (become
 * un-used). The scheduler handles the stopping and starting of waiting
 * processes.
 */
int syswait(int pid) {
	struct pcb *waiting = currproc();
	waiting->state = WAITING;
	waiting->waitpid = pid;
	return 0;
}

/*
 * Clears out the pcb of the process and notifies it's parent of the exit.
 */
int sysexit(int exitcode) {
	struct pcb *exitproc = currproc();
	exitproc->context.pc = 0;
	exitproc->context.sp = 0;
	exitproc->context.lr = 0;
	exitproc->context.r0 = 0;
	int i;
	if(maxpid == exitproc->pid) {
		for(i = maxpid; i >= 0; i--) {
			if(RUNNABLE == ptable[i].state || RESERVED == ptable[i].state) {
				maxpid = i;
				break;
			}
		}
	}
	free_stackspace(exitproc->rampg);
	exitproc->numchildren = 0;
	exitproc->pid = NULLPID;
/* If this process was the child of another, subtract it's number of children */
	if(exitproc->ppid != NULLPID) {
		pidproc(exitproc->ppid)->numchildren--;
	}
	exitproc->ppid = NULLPID;
	exitproc->waitpid = NULLPID;
	exitproc->state = UNUSED;
	exitproc->initflag = 1;
	strncpy(exitproc->name, "\0", 1);
/* Return the exit code to initshell. */
  pidproc(ispid)->context.r0 = exitcode;
	return exitcode;
}
