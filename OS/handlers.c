/**************************************************************************//**
 * @author  Ben Haubrich                                                      
 * @file    handlers.c                                                        
 * @date    May 18th, 2019                                                  
 * @details \b Synopsis: \n fault handlers                                                  
 *****************************************************************************/
#include <proc.h> /* for currproc(), scheduler() */
#include <types.h> /* for stdint.h */
#include <tm4c123gh6pm.h> /* Hardware register macros. */
#include <kernel_services.h> /* for syswrite(), sysexit() */
#include <syscalls.h> /* For syscall numbers */

//! @cond Doxygen_Suppress_Warning
/* From vectors.s */
extern void interrupt_enable(int);
extern void systick_context_save(struct pcb *);
extern void mstke_repair();
extern void switch_to_msp();
/* From context.s */
extern void swtch(uint32_t sp);
/* Function prototypes. */
void syst_handler() __attribute__((noreturn, naked));
//! @endcond

/**
 * @brief
 *   Puts the return value from system calls into the processes context.
 */
static inline void syscreturn(uint32_t val) {
	struct pcb *userproc = currproc();
	userproc->context.r0 = val;
}

/**
 * @brief
 *   Non-maskable Interrupt Handler
 */
void nmi_handler() {
	while(1);
}

/**
 * @brief
 *   Hard Fault Handler
 */
void hfault_handler() {
	int faultstat_vect, faultstat_forced, faultstat_dbg;
	faultstat_vect  = (NVIC_HFAULT_STAT_R & 1 << 1);
	faultstat_forced = (NVIC_HFAULT_STAT_R & 1 << 30);
	faultstat_dbg = (NVIC_HFAULT_STAT_R & 1 << 31);
/* Clear out HFAULT_STAT */
  NVIC_HFAULT_STAT_R |= (faultstat_vect | faultstat_forced | faultstat_dbg);
/* View locals with a debugger. */
	while(1);
}

/**
 * @brief
 *   Memory Management Handler.
 *
 * When a process attempts to write memory that doesn't belong to it, a memory
 * management fault occurs. This is likely to happen because of stack overflow
 * or de-referencing and writing to NULL.
 *
 * @param psp
 *   The process stack pointer
 * @param stack
 *   The stack that was being used when the fault was triggered. 0 for psp, 1
 *   for msp
 * @see Pg. 181, datasheet
 */
void mm_handler(uint8_t stack, uint32_t psp) {
  int mmarv, mlsperr, mstke, mustke, derr, ierr;
  uint32_t fault_addr;
 
  mmarv = (NVIC_FAULT_STAT_R & 1 << 7);
  mlsperr = (NVIC_FAULT_STAT_R & 1 << 5);
  mstke = (NVIC_FAULT_STAT_R & 1 << 4);
  mustke = (NVIC_FAULT_STAT_R & 1 << 3);
  derr = (NVIC_FAULT_STAT_R & 1 << 1);
  ierr = (NVIC_FAULT_STAT_R & 1);
/* Clear out the FAULT_STAT register */
  NVIC_FAULT_STAT_R |= (mmarv | mlsperr | mstke | mustke | derr | ierr);

  if (mmarv) {
    fault_addr = NVIC_MM_ADDR_R;
  }
  else {
    fault_addr = 0;
  }

  /* Repair the stack so we can return properly. */
  if(mstke) {
    mstke_repair(psp);
  }

/* We only want user processes to exit. If the kernel faults, it needs to be */
/* debugged and fixed. */
  if(stack) {
    while(1);
  }

  if(psp > stacktop(currproc()->rampg) || psp < stackbottom(currproc()->rampg)){
    syswrite("Stack overflow\n\r");
/* Kill the offending process. */
    return;
  }
  else {
    syswrite("Segmentation fault\n\r");
/* Kill the offending process. */
    return;
  }
/* Get rid of unused variable warnings. */
  fault_addr = fault_addr;
}

/** 
 * @brief
 *   Bus Fault Handler.
 *   
 * In the case of a bus fault, the memory address being accessed is one that
 * does not exist on the tm4c memory map.
 * @param stack
 *   The stack that was being used when the fault was triggered. 0 for psp, 1
 *   for msp
 */
void b_handler(uint8_t stack) {
	uint32_t fault_addr;
	uint32_t bfarv, blsperr, bstke, bustke, impre, precise, ibus;
/* Make sure memory contents are valid. */
	bfarv = (NVIC_FAULT_STAT_R & (1 << 15));
/* Get the address of the fault has a valid address. */
  if(bfarv) {
    fault_addr = NVIC_FAULT_ADDR_R;
  }
  else {
    fault_addr = 0;
  }
/* See Pg. 179, datasheet. */
	blsperr = (NVIC_FAULT_STAT_R & (1 << 13));
	bstke = (NVIC_FAULT_STAT_R & (1 << 12));
	bustke = (NVIC_FAULT_STAT_R & (1 << 11));
	impre = (NVIC_FAULT_STAT_R & (1 << 10));
	precise = (NVIC_FAULT_STAT_R & (1 << 9));
	ibus = (NVIC_FAULT_STAT_R & (1 << 8));
/* Clear the contents of the fault register */
  NVIC_FAULT_STAT_R |= (fault_addr | bfarv | blsperr | bstke | bustke | impre |
                         precise | ibus);
/* View locals with a debugger. */
  printk("Bus fault from process with pid %d\n\r", currproc()->pid);
  if(stack) {
    while(1);
  }
  return;
}

/**
 * @brief
 *   Usage Fault Handler.
 * @param stack
 *   The stack that was being used when the fault was triggered. 0 for psp, 1
 *   for msp
 */
void u_handler(uint32_t stack) {
	uint32_t div0, unalign, nocp, invpc, invstat, undef;
/* Divide by zero */
	div0 = (NVIC_FAULT_STAT_R & (1 << 25));
/* Unaligned memory access */
	unalign = (NVIC_FAULT_STAT_R & (1 << 24));
/* Processor attempted to access a coprocessor */
	nocp = (NVIC_FAULT_STAT_R & (1 << 19));
/* Processor attempted an illegal load of EXC_RETURN to the pc as a result */
/* of an invalid context or an invalid EXC_RETURN value. */
	invpc = (NVIC_FAULT_STAT_R & (1 << 18));
/* The processor has attempted to execute an instruction that makes illegal */
/* use of the epsr register. */
	invstat = (NVIC_FAULT_STAT_R & (1 << 17));
/* The processor has attemped to execute an undefined instruction. */
	undef = (NVIC_FAULT_STAT_R & (1 << 16));
/* Clear out FAULT_STAT. */
	NVIC_FAULT_STAT_R |= (div0 | unalign | nocp | invpc | invstat | undef);
  if(0 != div0) {
    syswrite("Divide by zero error\n\r");
  }
  if(stack) {
    while(1);
  }
  return;
}

/**
 * @brief
 *   syscall handler
 *
 * Supervisor Call (syscall) Handler. All SVC end up here, and then it's
 * decided how to handle it based on the sysnum.
 *
 * This function is naked because if it wasn't it would subtract 8 bytes from
 * the stack pointer and never add it back because it does not return.
 */
void svc_handler(int sysnum, void *arg1, void *arg2, void *arg3) {
  /* Return values from system calls. */
	uint32_t ret;

	switch(sysnum) {
		case FORK: ret = sysfork();
						break;
		case WAIT: ret = syswait(*((uint32_t *)arg1));
						break;
		case EXIT: sysexit(*((uint32_t *)arg1));
            return;
    case WRITE: ret = syswrite((char *)arg1);
            break;
    case LED: ret = sysled(*((int *)arg1), *((int *)arg2));
            break;
    case YEILD: sysyield();
            break;
    case SPI: sysspi(*((int *)arg1), ((uint8_t *)arg2));
            break;
		default: while(1); 
	}
/* Store return values */
	syscreturn(ret);
}

/**
 * @brief
 *   Debug monitor
 * @note
 *   disabled
 */
void dm_handler() {
	while(1);
}

/**
 * @brief
 *   PendSV handler
 */
void psv_handler() {
	while(1);
}

/**
 * Systick handler (clock tick interrupt)
 * @attention
 *   Scheduling is interruptable by the clock tick.
 * @pre
 *   Mode must be privledged and stack must be msp
 */
void syst_handler() {
  
  systick_context_save(currproc());
  switch_to_msp();

	if(WAITING == currproc()->state) {
/* Don't change the state to RUNNABLE for WAITING, just go to the scheduler */
		scheduler();
	}
	else {
		currproc()->state = RUNNABLE;
		scheduler();
	}
}
