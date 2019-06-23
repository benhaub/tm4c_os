#ifndef __PROC_H__
#define __PROC_H__
/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
/* 2KB blocks in 256KB of flash memory */
#define MAX_PROC 128

enum procstate {SLEEPING, RUNNABLE, RUNNING, UNUSED, EMBRYO};

/* Note that any changes to a processes context do not take affect until */
/* The next time a context switch changes to it. */
struct context {
	 unsigned long int r1;
	 unsigned long int r2;
	 unsigned long int r3;
	 unsigned long int r4;
	 unsigned long int r5;
	 unsigned long int r6;
	 unsigned long int r7;
	 unsigned long int r8;
	 unsigned long int r9;
	 unsigned long int r10;
	 unsigned long int r11;
	 unsigned long int sp;
	 unsigned long int lr;
	 unsigned long int pc;
};

/* Process control block. */
struct pcb {
	char name[16];
	int pid;
	enum procstate state;
	struct context context;
};

int allocproc(char[]);

#endif /*__PROC_H__*/
