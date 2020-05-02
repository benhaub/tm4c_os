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

/* From vectors.s */
extern void processor_state(int);
extern void kernel_entry(struct pcb *);
/* From context.s */
extern void swtch(word sp);
/* Function prototypes. */
void syst_handler(word) __attribute__((noreturn));

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
/* Bus Fault Handler. */
void b_handler() {
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
	while(1);
}
/* Usage Fault Handler. */
void u_handler() {
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
/* The processir has attemped  to execute an undefined instruction. */
	undef = (NVIC_FAULT_STAT_R & (1 << 16));
/* Eliminate unsed variable warnings. */
	div0=div0;unalign=unalign;nocp=nocp;invpc=invpc;invstat=invstat;undef=undef;
	while(1);
}
/* Supervisor Call (syscall) Handler. Acts as the OS Dispatcher. All SVC end */
/* up here, and then it's decided how to handle it based on the sysnum. */
/*TODO:
 * args should be void *. Change once write_flash and everything else has been
 * verified to work.
 */
void svc_handler(int sysnum, word arg1, word arg2, word arg3) {
/* Return values from system calls. */
	word ret;
/* Disable interrupts to prevent scheduling while performing kernel */
/* services. */
	processor_state(0);
	switch(sysnum) {
		case 0: ret = sysfork();
						break;
		case 1: ret = syswait(arg1);
						break;
		case 2: ret = sysexit(arg1);
						break;
    case 3: ret = sysflash(arg1, arg2, arg3);
            break;
		default: while(1); 
	}
/* Store return values */
	syscreturn(ret);
/* Re-enable interrupts. */
	processor_state(1);
}
void dm_handler() {
	while(1);
}
void psv_handler() {
	while(1);
}
/* Systick handler (clock tick interrupt) */
void syst_handler(word sp) {
	struct pcb *systproc = currproc();
/* Don't change the state to RUNNABLE, just go to the scheduler */
	if(UNUSED == systproc->state || WAITING == systproc->state) {
		kernel_entry(systproc);
		scheduler();
	}
	else {
		kernel_entry(systproc);
		systproc->state = RUNNABLE;
		scheduler();
	}
}
