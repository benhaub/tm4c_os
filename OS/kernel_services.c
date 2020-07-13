/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : kernel_services.c                                               *
 * Synopsis : Supported kernel services for users.                            *
 * Date     : July 18th, 2019                                                 *
 *****************************************************************************/
#include <proc.h>
#include <types.h>
#include <cstring.h>
#include <mem.h> /* in sysexit(), for free_stackspace() */
#include <hw.h> /* for write_flash() */

/*
 * IMPORTANT:
 *   Since all the system calls are executed in handler mode, any fault that
 *   is triggered here is an escalation to a hard fault.
 */

/* From proc.c */
extern int maxpid;
extern struct pcb ptable[];

/*
 * Write memory that starts at saddr and ends at eaddr to flash address faddr.
 * returns 0 on success, -1 otherwise.
 */
int sysflash(void *saddr, void *eaddr, void *faddr) {
  return write_flash(saddr, eaddr, faddr);
}

/*
 * Creates a new process. The parent forks the child. parent returns the pid
 * of the new process, child returns NULLPID. Returns -1 on failure.
 */
int sysfork() {
	struct pcb *child = reserveproc(NULL);
	if(NULL == child) {
		return -1;
	}
	struct pcb *parent = currproc();
  parent->numchildren++;
	child->context.pc = parent->context.pc;
/* Number of bytes being used in the parent stack */
  word pstackuse = stacktop(parent->rampg) - parent->context.sp;
/* Copy the parent's stack */
  memcpy(
      (void *)(stacktop(child->rampg) - pstackuse),
      (void *)(stacktop(parent->rampg) - pstackuse),
      pstackuse 
  );
/* Adjust the stack pointer of the child to the same offset as the parent. */
  child->context.sp -= pstackuse;
	child->ppid = parent->pid;
/* Forked will return NULLPID to the user process. */
	child->context.r0 = NULLPID;
	return child->pid;
}

/*
 * The calling process waits for the process belonging to pid to exit (become
 * un-used). The scheduler handles the stopping and starting of waiting
 * processes.
 */
int syswait(int pid) {
	struct pcb *waiting = currproc();
	waiting->state = WAITING;
	waiting->waitpid = pid;
	return 0;
}

/*
 * Clears out the pcb of the process and notifies it's parent of the exit.
 */
int sysexit(int exitcode) {
	struct pcb *exitproc = currproc();
	exitproc->context.pc = 0;
	exitproc->context.sp = 0;
	exitproc->context.lr = 0;
	exitproc->context.r0 = 0;
	int i;
	if(maxpid == exitproc->pid) {
		for(i = maxpid; i >= 0; i--) {
			if(RUNNABLE == ptable[i].state || RESERVED == ptable[i].state) {
				maxpid = i;
				break;
			}
		}
	}
	free_stackspace(exitproc->rampg);
	exitproc->pid = NULLPID;
/* If this process was the child of another, subtract it's number of children */
	if(exitproc->ppid != NULLPID) {
		pidproc(exitproc->ppid)->numchildren--;
	}
	exitproc->ppid = NULLPID;
  exitproc->waitpid = NULLPID;
  if(0 != exitproc->numchildren) {
    printf("Parent with pid %d exited with children\n\r", exitproc->numchildren);
  }
  exitproc->state = UNUSED;
	exitproc->initflag = 1;
	strncpy(exitproc->name, "\0", 1);
/* Return the exit code to the parent */
  if(exitcode != 0) {
    return 1;
  }
  return exitcode;
}
