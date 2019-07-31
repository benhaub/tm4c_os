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
int maxpid;
/* Array of processes for the scheduler. */
struct pcb ptable[MAX_PROC];
/* Pid of the current process. */
int currpid;

/*
 * Initializes the first user process and runs it.
 * param name
 * 	The name of the new process
 */
void user_init() {
/* Set all globals. Compiler doesn't seem to want to cooperate with global */
/* initializations of variables. */
	maxpid = 0;
	currpid = 0;
	struct pcb *initshell = reserveproc("initshell");
	initproc(initshell);
	scheduler();
}

/*
 * Reserve a process for further initialization and scheduling. Returns the
 * pcb of the reserved process.
 */
struct pcb* reserveproc(char *name) {
	if(sizeof(name) > 16 && NULL != name) {
		return NULL;
	}
/* Find an UNUSED process from the process table. */
	int i = 0;
	while(1) {
		if(ptable[i].state == UNUSED) {
			if(maxpid < i) {
				maxpid = i;
			}
			break;
		}
		else if(i > MAX_PROC) {
			return NULL;
		}
		else {
			i++;
		}
	}
	ptable[i].state = RESERVED;
	strncpy(name, ptable[i].name, strlen(name));
/* The pid is always the index where it was secured from. */
	ptable[i].pid = i;
	return (ptable + i);
}

/*
 * Initialize a RESERVED process so it's ready to be run by the scheduler
 * TODO:
 * I added a bunch of stuff to the proc struct? What should the initial values
 * be and where should the be initialized and where should we allow them to be
 * changed?
 */
void initproc(struct pcb *reserved) {
	reserved->state = EMBRYO;
	int i;
	if(reserved->numchildren != 0) {
		reserved->numchildren = 0;
	}
/* For every proc in the ptable. It's pid (or index in the ptable) determines*/
/* where it will reside in flash. The first process will reside in the second*/
/* block, the second process will reside in the third block. The kernel is */
/* located in the first block. swtch() will branch to this address, so bit[0]*/
/* must be 1 because EPSR has the thumb bit set on all armv7-m */
/* acrchitectures, hence why we add 1 to the address. All branches to */
/* link register in thumb mode must be to an address whose bit[0] is 1. */

/* The default value of all members in the context of new procs is */
/* initialized is zero. If they are not zero, it means they have been */
/* deliberately set to something else. */
	if(reserved->context.pc == 0) {
/* The addition of 1 to reserved->pid is in-case the pid is 0. */
		reserved->context.pc = ((reserved->pid + 1) * FLASH_PAGE_SIZE) + 1;
	}
/* Multiply by twice the stack size since the top of the stack at position */
/* 1 is 0x20002000, and decreases to 0x20001000. */
	if(reserved->context.sp == 0) {
		reserved->context.sp = _SRAM + ((get_stackspace()*STACK_SIZE)+STACK_SIZE);
	}
/* Leave room for the stack frame to pop into when swtch()'ed to. initcode */
/* will put the the sp at the top of the stack, then swtch() will put the sp */
/* into lr. */
	reserved->context.sp = ptable[reserved->pid].context.sp - 52;
	initcode(ptable[reserved->pid].context.sp, ptable[reserved->pid].context.pc);
	reserved->state = RUNNABLE;
}

/* Set all procs to unused state. */
void init_ptable() {
	int i;
	for(i = 0; i < MAX_PROC; i++) {
		ptable[i].state = UNUSED;
	}
}

/* Sets the pc and sp to zero. */
void init_context() {
	int i;
	for(i = 0; i < MAX_PROC; i++) {
		ptable[i].context.sp = ptable[i].context.pc = 0;
	}
}

/* Return the process that is currently RUNNING. */
struct pcb* currproc() {
	return (ptable + currpid);
}

/*
 * Return the process belonging to pid, or NULL if it couldn't be found.
 */
struct pcb* pidproc(int pid) {
	int i;
	for(i = 0; i < MAX_PROC; i++) {
		if(pid == ptable[i].pid) {
			return (ptable + i);
		}
	}
	return NULL;
}

/*
 * Round Robin scheduler. Triggered by tick interrupt every 1ms
 */
void scheduler() {
/* Current index of the scheduler. */
	static int index;
/* For initialization. arm-none-eabi-gcc initialises to -1 */
	if(index < 0) {
		index = 0;
	}
	while(1) {
/* Reset if we're looking passed the largest pid, there will be no RUNNABLE */
/* processes passed that index. */
		if(index > maxpid || index > MAX_PROC) {
			index = 0;
		}
		if(ptable[index].state == WAITING && ptable[index].
		else if(ptable[index].state == RUNNABLE) {
			currpid = ptable[index].pid;
			swtch(ptable[index].context.sp);
		}
		else {
			index++;
		}
	}
}	
