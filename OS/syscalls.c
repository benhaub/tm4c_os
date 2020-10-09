/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : syscalls.c                                                      *
 * Synopsis : System calls for tm4c_os kernel services                        *
 * Date     : July 18th, 2019                                                 *
 *****************************************************************************/
#include <types.h>
#include <proc.h> //For pcb struct and proc states
#include <syscalls.h> //Some functions have attributes

/* From syscall.s */
extern int syscall(int sysnum, struct pcb *);
extern int syscall1(int sysnum, struct pcb *, void *arg1);
extern int syscall2(int sysnum, struct pcb *, void *arg1, void *arg2);
extern int syscall3(int sysnum, struct pcb *, void *arg1, void *arg2, void *arg3);

/* See kernel_services.c for the implementation of each syscall. */

int fork() {
	return syscall(FORK, currproc());
}

int wait(int pid) {
	int ret;
  struct pcb *waitproc;

  waitproc = pidproc(pid);
/* No need to wait if the process is UNUSED. It may be in this state because */
/* it has already ran and finished before the parent made the call to wait */
/* for it. */
  if(NULL == waitproc || UNUSED == waitproc->state) {
    return 1;
  }
	waitproc = currproc();
	ret = syscall1(WAIT, waitproc, &pid);
/* Wait for state to change. This is done here because svc's are higher */
/* priority than systick exceptions so the tick interrupt gets masked out. */
/* Interrupts are allowed here. */
	while(WAITING == waitproc->state);
	return ret;
}

int exit(int exitcode) {
	syscall1(EXIT, currproc(), &exitcode);
/* Wait to be scheduled. This is done because all syscalls must return from */
/* the svc handler in order to leave handler mode. */
	while(1);
}

int write(char *msg) {
  return syscall1(WRITE, currproc(), msg);
}

int led(int colour, int state) {
  return syscall2(LED, currproc(), &colour, &state);
}
