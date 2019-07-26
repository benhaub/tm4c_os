/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.c
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <proc.h>
#include <types.h>

int sysfork() {
	struct pcb *forked = reserveproc(NULL);
	struct pcb forker = currproc();
/* Copy the the process that forked to the forked process. */

	return 0;
}
