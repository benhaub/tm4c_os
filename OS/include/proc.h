/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    proc.h                                                          
 * @date    June 5th, 2019                                                  
 * @details \b Synopsis: \n Process related structs and functions                           
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
 *   Do no not use MAX_PROC as a limit for process creation. Use NPROC instead.
 * @sa NPROC
 */
#define MAX_PROC 24
/**
 * @def NPROC
 *   The maximum number of creatable processes, which accounts for the
 *   the creation of userinit during OS initialization.
 */
#define NPROC MAX_PROC - 1
/**
 * @def NULLPID
 *   A pid that no valid process will ever have.
 */
#define NULLPID MAX_PROC + 1
/* Exit codes */
/**
 * @def EXIT_SUCCESS
 *   Send the exit success code to exit()
 * @sa sysexit
 */
#define EXIT_SUCCESS 0
/**
 * @def EXIT_FAILURE
 *   Send the exit failure code to exit()
 * @sa sysexit
 */
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
 * @enum procstate Process state
 * @var procstate::UNUSED
 * 	 The process is unused and can be reserved by a new process.
 * @var procstate::RESERVED
 * 	 The process has been reserved for use, but not initialised yet.
 * @var procstate::EMBRYO
 * 	 The process is midway through initialization
 * @var procstate::SLEEPING
 * 	 The process has been put to sleep and will not be run next scheduling cycle
 * @var procstate::RUNNABLE
 * 	 The process is ready to be scheduled
 * @var procstate::RUNNING
 * 	 The process is currently executing code
 * @var procstate::WAITING
 * 	 The processes is waiting for another process to exit
 */
enum procstate {UNUSED, RESERVED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, WAITING};

/**
 * @struct context
 *   Note that any changes to a processes context do not take affect until
 *   The next time a context switch changes to it. The registers here are
 *   based off of the ones saved the cortex M4 exception return stack
 *   r7 always needs the current stack pointer, and is taken care of in swtch
 * @var context::sp
 *   Stack pointer
 * @var context::pc
 *   Program counter
 * @var context::lr
 *   Link register
 * @var context::r0
 *   General purpose register 0
 * @var context::r3
 *   General purpose register 3
 * @var context::r7
 *   General purpose register 7
 * @var context::r12
 *   General purpose register 12
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
 * @warning
 *   Don't forget to initialize values in init_ptable if needed
 * @var pcb::context
 *   CPU register context. Do not re-order this member
 * @sa context
 * @var pcb::name
 *   For debugging
 * @var pcb::numchildren
 *   The number of child processes belonging to this process
 * @var pcb::pid
 *   Process ID
 * @var pcb::ppid
 *   Parent process ID.
 * @var pcb::waitpid
 *   Process is waiting for this pid to change state.
 * @var pcb::initflag
 *   0 for not initialised yet, 1 for initialised
 * @sa scheduler
 * @var pcb::rampg
 *   Index of this processes allocated ram page.
 * @sa get_stackpage
 * @var pcb::state
 * @sa procstate
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
