/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.c
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <proc.h>
#include <types.h>

/*
 * Creates a new process. The forker forks the forked. */
int sysfork() {
	struct pcb *forked = reserveproc(NULL);
	struct pcb *forker = currproc();
/* Copy the the process that forked to the forked process. */
	forked->context.pc = forker->context.pc;
	forked->context.r0 = NULLPID;
	initproc(forked);
	return 0;
}
