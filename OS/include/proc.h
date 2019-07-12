#ifndef __PROC_H__
#define __PROC_H__
/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
#include <types.h>
/* 2KB blocks in 256KB of flash memory, minus the space for kernel */
#define MAX_PROC 127

enum procstate {SLEEPING, RUNNABLE, RUNNING, UNUSED, EMBRYO};

/* Note that any changes to a processes context do not take affect until */
/* The next time a context switch changes to it. */
struct context {
	 word sp;
	 word lr;
	 word pc;
};

/* Process control block. */
struct pcb {
	struct context context;
	char name[16];
	int pid;
	enum procstate state;
};

int user_init(void);
int allocproc(char[]);
void init_ptable(void);
struct pcb currproc(void);

#endif /*__PROC_H__*/
