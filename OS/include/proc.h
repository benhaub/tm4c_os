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
/* Exit codes */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
/* Maximum number of child processes. */
#define MAX_CHILD 4

/*
 * UNUSED:
 * 	The process is unused and can be reserved by a new process.
 * RESERVED:
 * 	The process has been reserved for use, but not initialised yet.
 * EMBRYO:
 * 	The process is midway through initialization
 * SLEEPING:
 * 	The process has been put to sleep and will not be run next scheduling cycle
 * RUNNABLE:
 * 	The process is ready to be scheduled
 * RUNNING:
 * 	The process is currently executing code
 * WAITING:
 * 	The processes is waiting for another process to exit
 */
enum procstate {UNUSED, RESERVED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, WAITING};

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
	struct context context; /* CPU register contentst */
	char name[16];	/* For debugging */
	int children[MAX_CHILD]; /* Hold pid's of children. */
	int numchildren; /* Number of child processes. */
	int pid; /* Process ID */
	int ppid; /* Parent process ID */
	enum procstate state; /* Process state */
};

void user_init(void);
struct pcb* reserveproc(char *);
void initproc(struct pcb *);
void init_ptable(void);
void init_context(void);
struct pcb *currproc(void);
struct pcb *pidproc(int);
void scheduler(void);

#endif /*__PROC_H__*/
