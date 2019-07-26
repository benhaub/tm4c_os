/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.c
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <types.h>
#include <proc.h>
/* Syscall numbers */
#define FORK 0

/* From syscall.s */
extern int syscall(int sysnum, struct pcb *);

int fork() {
	return syscall(FORK, currproc());
}

