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
 * the registers from the process that is running.
 * void swtch(word sp), where sp is the top of the stack of the process to
 * switch to.
 */
	.global swtch
	.type swtch, %function
swtch: .fnstart
				add r1, r0, #4
				ldr r2, [r1] //context.pc
				ldr r3, [r0] //context.sp
/* Store the pc to it's popped to the lr */
				str r2, [r3, #20] 
				add r1, r0, #12
				ldr r2, [r1] //context.r0
/* Store r0 on the stack */
				str r2, [r3] 
/* Switch psp stack to new one */
				msr psp, r3 
/* Switch to psp and unprivledge mode. */
				mrs r2, CONTROL
				orr r2, r2, #0x3
				msr CONTROL, r2
				pop {r0-r3, r12, r14}
				bx lr
				.fnend
	.end
	
