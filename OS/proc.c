/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
#include <mem.h>
#include <proc.h>
#include <cstring.h>
#include <tm4c123gh6pm.h>

/* From context.s */
extern void swtch(word);
extern void initcode(word sp, word pc);
/* The largest pid currently RUNNING. Keeping track of this speeds up */
/* scheduling. */
int maxpid = 0;
/* Array of processes for the scheduler. */
struct pcb ptable[MAX_PROC];
/* Pid of the current process. */
int currpid = 0;
/*
 * Returns the index of the found process in the process table.
 * param name
 * 	The name of the new process
 * returns -1 on error, index of the new process on success.
 */
int user_init() {
	int pid = allocproc("initShell");
	/* return to user space. See context.s */
	swtch(ptable[pid].context.sp);
	return 0;
}
/*
 * Secure an UNUSED process from the process table and return the index of the
 * process that was used.
 */
int allocproc(char *name) {
	if(sizeof(name) > 16) {
		return -1;
	}
/* Find an UNUSED process from the process table. */
	int i = 0;
	while(i < MAX_PROC) {
		if(ptable[i].state == UNUSED) {
			if(maxpid < i) {
				maxpid = i;
			}
			break;
		}
		else {
			i++;
		}
	}
/* Initialize the pcb. */
	ptable[i].state = EMBRYO;
/* The pid is always the index where it was secured from. */
	ptable[i].pid = i;
/* For every proc in the ptable. It's index determines where it will reside */
/* in flash. The first process will reside in the second block, the second */
/* process will reside in the third block. The kernel is located in the */
/* first block. swtch() will branch to this address, so bit[0] must be */
/* 1 because EPSR has the thumb bit set on all a4mv7-m acrchitectures, hence */
/* why we add 1 to the address. */
	ptable[i].context.pc = ((i + 1) * FLASH_PAGE_SIZE) + 1;
/* Multiply by twice the stack size since the top of the stack at position */
/* 1 is 0x20002000, and decreases to 0x20001000. */
	ptable[i].context.sp = _SRAM + ((get_stackspace() * STACK_SIZE) + STACK_SIZE);
/* Leave room for the stack frame to pop into when swtch()'ed to. Init code */
/* will put the the sp at the top of the stack, then swtch() will put the sp */
/* into lr. */
	ptable[i].context.sp = ptable[i].context.sp - 52;
	strncpy(name, ptable[i].name, strlen(name));
	initcode(ptable[i].context.sp, ptable[i].context.pc);
	ptable[i].state = RUNNABLE;
	return i;
}	
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
/* Reset if we're looking passed the largest pid, there will be no RUNNABLE */
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
