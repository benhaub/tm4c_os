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
/* A pid that no valid process will ever have. */
#define NULLPID MAX_PROC + 1

enum procstate {UNUSED, RESERVED, EMBRYO, SLEEPING, RUNNABLE, RUNNING};

/* Note that any changes to a processes context do not take affect until */
/* The next time a context switch changes to it. */
struct context {
	 word sp;
	 word pc;
	 word lr;
	 word r0;
	 word r1;
	 word r2;
	 word r3;
};

/* Process control block. */
struct pcb {
	struct context context;
	char name[16];
	int pid;
	enum procstate state;
};

void user_init(void);
struct pcb* reserveproc(char *);
void initproc(struct pcb *);
void init_ptable(void);
void init_context(void);
struct pcb *currproc(void);
void scheduler(void);

#endif /*__PROC_H__*/
