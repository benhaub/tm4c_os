/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.c
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <proc.h>
#include <types.h>

/*
 * Creates a new process. The forker forks the forked. Forker returns the pid
 * of the new process, forked returns NULLPID
 */
int sysfork() {
	struct pcb *forked = reserveproc(NULL);
	struct pcb *forker = currproc();
/* Copy the the process that forked to the forked process. */
	forked->context.pc = forker->context.pc;
	forked->context.r0 = NULLPID;
	initproc(forked);
	return forked->pid;
}

/*
 * The calling process waits for the process belonging to pid to exit (become
 * un-used).
 */
int syswait(int pid) {
/*TODO:
 * Is waiting possible for single core or do we have to implement syscheck()
 * instead?
 */
	struct pcb *waitfor = pidproc(pid);
	return 0;
}

int sysexit() {
	return 0;
}
