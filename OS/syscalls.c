/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.c
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#include <types.h>
/* Syscall numbers */
#define FORK 0

/* From syscall.s */
extern int syscall(int sysnum);

int fork() {
	return syscall(FORK);
}

