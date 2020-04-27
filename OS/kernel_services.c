/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : kernel_services.c                                               *
 * Synopsis : Supported kernel services for users.                            *
 * Date     : July 18th, 2019                                                 *
 *****************************************************************************/
#include <proc.h>
#include <types.h>
#include <cstring.h>
#include <mem.h>

/* From proc.c */
extern int maxpid;
extern struct pcb ptable[];

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
	if(forker->numchildren < MAX_CHILD) {
		forker->numchildren++;
	}
	else {
		/* This process has already forked the max number of children. */
		return -1;
	}
/* Copy context from the process that forked to the forked process. */
	forked->context.pc = forker->context.pc;
/* Copy the processes memory block that forked to the forked process */
/* and make sure that we don't copy into the previous ram page */
/* If this statement is true, it means we've run out of stack space */
	if(forker->context.sp - 24 < (forker->rampg - 1)*STACK_SIZE) {
		return -1;
	}
/* Fork copies 24 bytes of the parents stack to the child. There's no good */
/* reason for choosing 24. You might be able to get away with less, maybe it */
/* needs more. I don't know the answer right now. initcode will overwrite */
/* any values it needs to later on. */
	memcpy((word *)(forked->rampg*STACK_SIZE),
					(word *)forker->context.sp, 
					24);
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
/* Return the exit code to the parent */
	return exitcode;
}
