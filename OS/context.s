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
 * void swtch(word sp), where sp is the top of the stack of the process to
 * switch to.
 */

/*
 *TODO:
 * This context switcher is totally useless. It doesn't save the previous
 * processes context at all. It pushes the kernel context onto the stack
 * which doesn't need to be done, and it's stuck in handler mode. This needs
 * a huge rework.
 */
	.global swtch
	.type swtch, %function
swtch: .fnstart
				push {r0-r12, r14}
				msr psp, r0
/* Switch to psp and unprivledge mode. */
				mrs r0, CONTROL
				orr r0, r0, #0x3
				msr CONTROL, r0 /* Now unprivledged and using psp */
				pop {r0-r12}
				pop {r14}
				bx lr
				.fnend

/*
 * All new processes run initcode first to set up cpu registers to make it
 * look as if the process had been interrupted by an svc with it's registers
 * pushed on the stack. initcode places the value of pc into the link register.
 * initcode(word sp)
 */
	.global initcode
	.type initcode, %function
initcode: .fnstart
					add r1, r0, #4
					ldr r2, [r1] //context.pc
					ldr r3, [r0] //context.sp
					str r2, [r3, #52] //store pc on the stack
					add r1, r0, #12
					ldr r2, [r1] //context.r0
					str r2, [r3] //store r0 on the stack
					bx lr
					.fnend	
	.end
	
