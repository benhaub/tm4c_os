/******************************************************************************
 *Authour : Ben Haubrich                                                      *
 *File    : handlers.c                                                        *
 *Synopsis: fault handlers                                                    *
 *Date    : May 18th, 2019                                                    *
 *****************************************************************************/
#include <tm4c123gh6pm.h> /* Hardware register macros. */
#include <types.h> /* For the word data type. */
#include <kernel_services.h> /* Syscalls for svc_handler. */
#include <proc.h> /* In systick interrupt, For scheduler() */
#include <cstring.h> /* For printf() */

/* From vectors.s */
extern void processor_state(int);
extern void kernel_entry(struct pcb *);
/* From context.s */
extern void swtch(word sp);
/* Function prototypes. */
void syst_handler(word) __attribute__((noreturn, naked));

/*
 * Puts the return value from system calls into the processes context.
 */
static void syscreturn(word val) {
	struct pcb *userproc = currproc();
	userproc->context.r0 = val;
}

void nmi_handler() {
	while(1);
}
void hfault_handler() {
	int faultstat_vect, faultstat_forced, faultstat_dbg;
	faultstat_vect  = (NVIC_HFAULT_STAT_R & 1 << 1);
	faultstat_forced = (NVIC_HFAULT_STAT_R & 1 << 30);
	faultstat_dbg = (NVIC_HFAULT_STAT_R & 1 << 31);
/* Eliminate unsed variable warnings. */
	faultstat_vect=faultstat_vect;faultstat_forced=faultstat_forced;
	faultstat_dbg=faultstat_dbg;
  NVIC_HFAULT_STAT_R |= 0xFFFFFFFF;
	while(1);
}
/* Memory Management Handler. */
void mm_handler() {
	while(1);
}
/* 
 * Bus Fault Handler. Code that generates a bus fault usually exhibits
 * similar behaviour to code running in Linux that generates a segmentation
 * fault (accessing illegal adresses).
 * @param stack
 *   The stack that was being used when the fault was triggered. 2 for psp, 1
 *   for msp
 */
void b_handler(int stack) {
	word fault_addr;
	word bfarv, blsperr, bstke, bustke, impre, precise, ibus;
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
/* Eliminate unsed variable warnings. */
	fault_addr=fault_addr;bfarv=bfarv;blsperr=blsperr;bstke=bstke;bustke=bustke;
	impre=impre;precise=precise;ibus=ibus;
/* Clear the contents of the fault register */
  NVIC_HFAULT_STAT_R |= 0xFFFFFFFF;
/* View the contents with a debugger. */
  printf("Bus Fault\n\r");
  if(2 == stack) {
    return;
  }
  while(1);
}
/*
 * Usage Fault Handler.
 * @param stack
 *   The stack that was being used when the fault was triggered. 2 for psp, 1
 *   for msp
 */
void u_handler(int stack) {
	word div0, unalign, nocp, invpc, invstat, undef;
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
/* Eliminate unused variable warnings. */
	unalign=unalign;nocp=nocp;invpc=invpc;invstat=invstat;undef=undef;
  if(0 != div0) {
    printf("Divide by zero error\n\r");
  }
  if(2 == stack) {
    return;
  }
  while(1);
}
/* Supervisor Call (syscall) Handler. Acts as the OS Dispatcher. All SVC end */
/* up here, and then it's decided how to handle it based on the sysnum. */
void svc_handler(int sysnum, void *arg1, void *arg2, void *arg3) {
/* Return values from system calls. */
	word ret;
	switch(sysnum) {
		case 0: ret = sysfork();
						break;
		case 1: ret = syswait(*((word *)arg1));
						break;
		case 2: ret = sysexit(*((word *)arg1));
						break;
    case 3: ret = sysflash(arg1, arg2, arg3);
            break;
		default: while(1); 
	}
/* Store return values */
	syscreturn(ret);
}
void dm_handler() {
	while(1);
}
void psv_handler() {
	while(1);
}
/* Systick handler (clock tick interrupt) */
void syst_handler(word sp) {
/* Don't change the state to RUNNABLE, just go to the scheduler */
  NVIC_ST_CURRENT_R = 0;
	if(UNUSED == currproc()->state || WAITING == currproc()->state) {
		kernel_entry(currproc());
		scheduler();
	}
	else {
		kernel_entry(currproc());
		currproc()->state = RUNNABLE;
		scheduler();
	}
}
