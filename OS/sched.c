/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 9th, 2019
 *****************************************************************************/
#include <proc.h>

struct pcb ptable[MAX_PROC];

/* Set all procs to unused state. */
void init_ptable() {
	int i;
	for(i = 0; i < MAX_PROC; i++) {
		ptable[i].state = UNUSED;
	}
}
