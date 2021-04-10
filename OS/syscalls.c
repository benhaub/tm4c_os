/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    syscalls.c                                                     
 * @date    July 18th, 2019                                               
 * @details \b Synopsis: \n System calls for tm4c_os kernel services                      
 *****************************************************************************/
#include <proc.h> //For pcb struct and proc states
#include <syscalls.h> //Some functions have attributes

/* From syscall.s */
/**
 * @brief
 *   For system calls with no arguments
 * @param sysnum
 *   The kernel service you are requesting. See syscall.h
 * @param pcb
 *   This parameter should always be set to currproc
 * @sa currproc
 */
extern int syscall(int sysnum, struct pcb *);
/** @sa syscall */
extern int syscall1(int sysnum, struct pcb *, void *arg1);
/** @sa syscall */
extern int syscall2(int sysnum, struct pcb *, void *arg1, void *arg2);
/** @sa syscall */
extern int syscall3(int sysnum, struct pcb *, void *arg1, void *arg2, void *arg3);

/* See kernel_services.c for the implementation of each syscall. */

/**
 * @brief
 *   Create a new process
 *
 * Creates a new process. The parent forks the child. 
 *
 * @return
 *   Parent returns the pid of the new process, child returns NULLPID.
 *   Returns 0 on failure.
 */
pid_t fork() {
	return syscall(FORK, currproc());
}

/**
 * @brief
 *   Wait for the process with pid to exit
 * @param pid
 *   The pid of the process to wait for
 * @return
 *   -1 on failure, 0 on success.
 */
int wait(pid_t pid) {
	int ret;
  struct pcb *waitproc;

  if(ptable_index_from_pid(pid) >= MAX_PROC) {
    return -1;
  }
  else if (pid == currproc()->pid) {
    return -1;
  }

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
//TODO: Add a yield call so we don't spin and wait.
	while(WAITING == waitproc->state);
	return ret;
}

/**
 * @brief
 *   Exit a process
 *
 * The process that calls this function will exit and it's place in the ptable
 * will transition to UNUSED.
 *
 * @param exitcode
 *   The exit status to exit with.
 *
 * @sa procstate EXIT_SUCCESS EXIT_FAILURE
 */
int exit(pid_t exitcode) {
	syscall1(EXIT, currproc(), &exitcode);
/* Wait to be scheduled. This is done because all syscalls must return from */
/* the svc handler in order to leave handler mode. */
	while(1);
}

/**
 * @brief
 *   Write bytes to the terminal output
 * @param msg
 *   The string to write
 * @return
 *   -1 on failure, 0 on success
 */
int write(char *msg) {
  return syscall1(WRITE, currproc(), msg);
}

/**
 * @brief
 *   Turn off/on the LED with the colour given.
 *
 * Multiple colours may be on at the same time. For example, if you turn on the
 * red led and then turn on the blue led you will get a purple light. Red must
 * be turned off to produce a blue at this point.
 *
 * @sa led_colours
 * @param colour
 *   The colour to turn off/on
 * @param state
 *   The state which the colour should be set to (on/off)
 */
int led(int colour, int state) {
  return syscall2(LED, currproc(), &colour, &state);
}
