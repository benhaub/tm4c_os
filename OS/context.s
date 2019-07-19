/****************************************************************************
 *Authour	:	Ben Haubrich																										*
 *File		:	cpntext.s																												*
 *Synopsis:	Context switching and processor privledge control								*
 *Date		: June 25th, 2019																									*
 ****************************************************************************/
	
	.syntax unified
	.thumb

/*
 * Context switcher. Save the currently running processes registers, and load
 * the registers from the process that is running. Function prototype is as
 * follows: 
 * void swtch(word sp), where sp is the top of the stack to load psp
 * with and pc is the location to start execution.
 */
	.global swtch
	.type swtch, %function
swtch: .fnstart
/* Assume for now that SVC will not keep us in handler mode. */
/* That means we will be in privledge mode, and be using psp. */
				push {r0-r12, r14}
				msr psp, r0
/* Switch to psp and thread mode using the procedure on Pg. 23 of cortex m4 */
/* generic user guide. */
				mrs r0, CONTROL
				orr r0, r0, #0x3
				msr CONTROL, r0 /* Now unprivledged and using psp */
				pop {r0-r12, r14}
				bx lr
				.fnend

/*
 * All new processes run initcode first to set up cpu registers
 * initcode(word sp, word pc)
 */
	.global initcode
	.type initcode, %function
initcode: .fnstart
					str r1 , [r0, #52]
					bx lr
					.fnend	
	.end
	
