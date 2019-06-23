/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
#include <mem.h>
#include <proc.h>
#include <string.h>

/* From sched.c */
extern struct pcb ptable[];
int nextpid = 1;

/*
 * Returns the index of the found process in the process table.
 * param name
 * 	The name of the new process
 * returns -1 on error, index of the new process on success.
 */
int allocproc(char *name) {
	if(sizeof(name) > 16) {
		return -1;
	}
/* Find an UNUSED process from the process table. */
	int i;
	struct pcb newproc;
	for(i = 0; i < MAX_PROC; i++) {
		if(ptable[i].state == UNUSED) {
/* Initialize the pcb. */
			newproc = ptable[i];
			newproc.state = EMBRYO;
			newproc.pid = nextpid++;
/* For every proc in the ptable. It's index determines where it will reside */
/* in flash. The first process will reside in the second block, the second */
/* process will reside in the third block. The kernel is located in the */
/* first block. */	
			newproc.context.pc = (i * FLASH_PAGE_SIZE);
/* Multiply by twice the stack size since the top of the stack at position */
/* 1 is 0x2000, and decreases to 0x1000. */
			newproc.context.sp = (get_stackspace() * (2 * STACK_SIZE));
			strncpy(name, newproc.name, strlen(name));
		}
	}
	return i;
}	
