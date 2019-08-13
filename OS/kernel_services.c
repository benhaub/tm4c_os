/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.c
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <proc.h>
#include <types.h>
#include <cstring.h>

/*
 * Creates a new process. The forker forks the forked. Forker returns the pid
 * of the new process, forked returns NULLPID
 */
int sysfork() {
	struct pcb *forked = reserveproc(NULL);
	struct pcb *forker = currproc();
	forked->numchildren++;
	forked->children[forked->numchildren] = forker->pid;
/* Copy the the process that forked to the forked process. */
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
 * TODO:
 * Need to adjust the array for stackspace
 */
int sysexit() {
	struct pcb *exitproc = currproc();
	exitproc->context.pc = 0;
	exitproc->context.sp = 0;
	exitproc->context.lr = 0;
	exitproc->context.r0 = 0;
	int i;
	for(i = 0; i < MAX_CHILD; i++) {
		exitproc->children[i] = NULLPID;
	}
	exitproc->numchildren = 0;
	exitproc->pid = 0;
	exitproc->ppid = 0;
	exitproc->waitpid = NULLPID;
	exitproc->state = UNUSED;
	exitproc->initflag = 0;
	strncpy(exitproc->name, "\0", 1);
	scheduler();
	return 0;
}
