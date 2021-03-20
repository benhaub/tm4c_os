/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : proc.h                                                          *
 * Synopsis : Process related structs and functions                           *
 * Date     : June 5th, 2019                                                  *
 *****************************************************************************/
#ifndef __PROC_H__
#define __PROC_H__

#include <types.h>
#include <mem.h>

/**
 * @def MAX_PROC
 *   Every process needs a stack, so max processes is how many stacks can fit
 *   in ram at the same time. The size of the kernel can not be pre-processed
 *   since it's calculated at startup in RESET_EXCP. The OS will warn you if
 *   MAX_PROC is defined to be too large by doing a single runtime check in
 *   user_init.
 * @warning
 *   Do no not use MAX_PROC as a limit for process creation.
 */
#define MAX_PROC 24
/**
 * @def NPROC
 *   The maximum number of creatable processes, which accounts for the
 *   the creation of initshell during OS initialization.
 */
#define NPROC MAX_PROC - 1
/**
 * @def NULLPID
 *   A pid that no valid process will ever have.
 */
#define NULLPID MAX_PROC + 1
/* Exit codes */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* Macro functions */
/**
 * @def ptable_index_from_pid
 *   Given a pid, return the process index in the the ptable that it identities.
 *   Therefore, this function should be a conversion from a pid to a ptable
 *   index for that processes pid.
 */
#define ptable_index_from_pid(p) p-1

/**
 * @enum procstate
 * @var KERNEL:
 *   The space for this process is being used by the kernel and is unavailable
 * 	 for use.
 * @var UNUSED:
 * 	 The process is unused and can be reserved by a new process.
 * @var RESERVED:
 * 	 The process has been reserved for use, but not initialised yet.
 * @var EMBRYO:
 * 	 The process is midway through initialization
 * @var SLEEPING:
 * 	 The process has been put to sleep and will not be run next scheduling cycle
 * @var RUNNABLE:
 * 	 The process is ready to be scheduled
 * @var RUNNING:
 * 	 The process is currently executing code
 * @var WAITING:
 * 	 The processes is waiting for another process to exit
 */
enum procstate {UNUSED, RESERVED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, WAITING};

/**
 * @struct context
 *   Note that any changes to a processes context do not take affect until
 *   The next time a context switch changes to it. The registers here are
 *   based off of the ones saved the cortex M4 exception return stack
 *   r7 always needs the current stack pointer, and is taken care of in swtch
 */
struct context {
	 uint32_t sp;
	 uint32_t pc;
	 uint32_t lr;
	 uint32_t r0;
	 uint32_t r3;
   uint32_t r7;
	 uint32_t r12;
};

/**
 * @struct pcb
 *   Process control block.
 * @var context
 *   @sa context. CPU register context. Do not re-order this member
 * @var name
 *   For debugging
 * @var pid
 *   Process ID
 * @var ppid
 *   Parent process ID.
 * @var waitpid
 *   Process is waiting for this pid to change state.
 * @var initflag
 *   0 for not initialised yet, 1 for initialised
 * @var rampg
 *   Index of this processes allocated ram page.
 * @var state
 *   @sa procstate
 * @warning
 *   Don't forget to initialise values in init_ptable if needed
 */
struct pcb {
	struct context context; 
	char name[16];
	int numchildren;
	pid_t pid;
	pid_t ppid;
	pid_t waitpid;
	uint8_t initflag;
	uint8_t rampg;
	enum procstate state;
};

void user_init(void);
struct pcb* reserveproc(char *);
void init_ptable(void);
struct pcb *currproc(void);
struct pcb *pidproc(int);
void scheduler(void) __attribute__((noreturn));

#endif /*__PROC_H__*/
