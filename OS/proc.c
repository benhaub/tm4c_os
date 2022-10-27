/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file     proc.c                                                          
 * @date     June 5th, 2019                                                 
 * @details \b Synopsis: \n Process related structs and functions                           
 *****************************************************************************/
#include <mem.h>
#include <proc.h>
#include <cstring.h>
#include <tm4c123gh6pm.h>
#include <kernel_services.h>
#include <scheduling_algorithm.h>

//! @cond Doxygen_Suppress_Warning
/* From context.s */
extern void swtch(uint32_t);
extern void initcode(uint32_t);

/** Array of processes for the scheduler. */
struct pcb ptable[MAX_PROC];
/** Pid of the current process. */
static unsigned int currpid;
//! @endcond

/**
 * @brief
 *   Initializes the first user process and runs it.
 * @sa mem.c
 *   for an explantion of calculations made
 */
void user_init() {
  if(MAX_PROC > SRAM_PAGES - (*((uint32_t *)(KRAM_USE - 4)) + 1)) {
    printk("MAX_PROC is set to allow more processes than the available RAM "\
        "can hold. Please use a value no greater than %d\n\r", \
       SRAM_PAGES - (*((uint32_t *)(KRAM_USE - 4)) + 1));
    return;
  }
  else if(MAX_PROC < SRAM_PAGES - (*((uint32_t *)(KRAM_USE - 4)) + 1)) {
    printk("Currently capping %d/%d available processes\n\r", MAX_PROC, \
      SRAM_PAGES - (*((uint32_t *)(KRAM_USE - 4)) + 1) - 1);
  }
/* Set all globals. Compiler doesn't seem to want to cooperate with global */
/* initializations of variables. */
	currpid = 0;
	struct pcb *userinit = reserveproc("userinit");
	if(NULL == userinit) {
		return;
	}
	userinit->context.pc = (uint32_t)main;
	scheduler();
}

/**
 * Reserve a process for further initialization and scheduling. Returns the
 * pcb of the reserved process.
 * @param name
 *   The name for the process
 */
struct pcb* reserveproc(char *name) {
	int rampg;
	int i = 0;

	if(NULL != name && strlen(name) > 16) {
    printk("Buffer overrun for process name\n\r");
		return NULL;
	}
/* Find an UNUSED process from the process table. */
	for(i = 0; i < MAX_PROC; i++) {
		if(UNUSED == ptable[i].state) {
			break;
		}
  }
  if(i >= MAX_PROC) {
    printk("No unused proc's in ptable\n\r");
    return NULL;
  }
/* Top of stack for this process. */
	if(-1 != (rampg = get_stackpage())) {
		ptable[i].rampg = rampg;
		ptable[i].context.sp = stacktop(rampg);
	}
	else {
    free_stackpage(rampg);
    ptable[i].context.sp = 0;
		return NULL;
	}
	ptable[i].state = RESERVED;
  if(NULL != name) {
    strncpy(ptable[i].name, name, strlen(name));
  }
/* The pid is always the index where it was secured from. */
	ptable[i].pid = i+1;
	return (ptable + i);
}

/**
 * Initialize a RESERVED process so it's ready to be context switched too.
 * This function alters context, so must it be run just before the context
 * switcher.
 */
static void initproc(struct pcb *reserved) {
	reserved->state = EMBRYO;
  reserved->context.r7 = reserved->context.sp;
/* Leave room for the stack frame to pop into when swtch()'ed to. initcode */
/* will put the the sp at the top of the stack, then swtch() will put the sp */
/* into lr. */
	reserved->context.sp = reserved->context.sp - (CTXSTACK);
/* Pointer to proc is cast to a uint32_t because the compiler didn't seem to */
/* want to give me the pointer. It always came out to the value of sp in */
/* context struct. */
	initcode((uint32_t)reserved);
	reserved->state = RUNNABLE;
}

/**
 * @brief
 *   Initialized the process table
 * @post
 *   Set all unused procs to unused state and initialize pcb members to known
 *   values.
 */
void init_ptable() {
	for(int i = 0; i < MAX_PROC; i++) {
		ptable[i].state = UNUSED;
		ptable[i].numchildren = 0;
    ptable[i].waitpid = NULLPID;
		ptable[i].ppid = NULLPID;
    ptable[i].pid = NULLPID;
		ptable[i].initflag = 1;
		ptable[i].context.sp = ptable[i].context.pc = 0;
	}
}

/**
 * @brief
 *   Return the process the is currently running
 * @return
 *   the process that is currently RUNNING.
 */
inline struct pcb* currproc() {
	return (ptable + ptable_index_from_pid(currpid));
}

/**
 * @brief
 *   Return the process whose pid is pid
 * @return
 *   Return the process belonging to pid, or NULL if it couldn't be found. One
 *   example of a pid that can't be found is the pid of a process that has
 *   exited.
 */
struct pcb* pidproc(int pid) {
	int i;
  if(pid >= MAX_PROC) {
    return NULL;
  }
	for(i = 0; i < MAX_PROC; i++) {
		if(pid == ptable[i].pid) {
			return (ptable + i);
		}
	}
	return NULL;
}

/**
 * @brief
 *   Triggered by tick interrupt every everytime the
 *   the system clock reaches zero.
 * @see start_clocktick
 * @see scheduling_algorithm.c
 */
void scheduler() {
  struct pcb *schedproc;
  while(1) {
/* Reset if we're looking passed the largest pid, there will be no RUNNABLE */
/* processes passed that index. */
    schedproc = round_robin();

/* If the process is waiting for another, check to see if it's exited. */
		if(schedproc->state == WAITING && \
			ptable[ptable_index_from_pid(schedproc->waitpid)].state == UNUSED) {
			schedproc->state = RUNNABLE;
			schedproc->waitpid = NULLPID;
		}
		if(schedproc->state == RESERVED || schedproc->state == RUNNABLE) {
			if(1 == schedproc->initflag) {
				initproc(schedproc);
				schedproc->initflag = 0;
			}
			currpid = schedproc->pid;
			schedproc->state = RUNNING;
      create_user_memory_region(schedproc->rampg);
			swtch(schedproc->context.sp);
		}
	}
}	
