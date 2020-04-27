/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : syscalls.c                                                      *
 * Synopsis : System calls for tm4c_os kernel services                        *
 * Date     : July 18th, 2019                                                 *
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
	int ret;
	struct pcb *waitproc = currproc();
	ret = syscall1(WAIT, waitproc, &pid);
/* Wait for state to change. This is done here because privledged code */
/* disables interrupts, so the tick interrupt gets masked out. Interrupts are */
/* allowed here. */
	while(WAITING == waitproc->state);
	return ret;
}

int exit(int exitcode) {
	syscall1(EXIT, currproc(), &exitcode);
/* Wait to be scheduled. This is done because the scheduler can't be called */
/* from handler mode. All sys*() calls run in handler mode. */
	while(1);
}

