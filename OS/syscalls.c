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
#define WAIT 1
#define EXIT 2

/* From syscall.s */
extern int syscall(int sysnum, struct pcb *);
extern int syscall1(int sysnum, struct pcb *, void *arg1);

int fork() {
	return syscall(FORK, currproc());
}

int wait(int pid) {
	return syscall1(WAIT, currproc(), &pid);
}

int exit() {
	return syscall(EXIT, currproc());
}

