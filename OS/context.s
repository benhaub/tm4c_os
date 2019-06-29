/****************************************************************************
 *Authour	:	Ben Haubrich																										*
 *File		:	cpntext.s																												*
 *Synopsis:	Context switching and processer privledge control								*
 *Date		: June 25th, 2019																									*
 ****************************************************************************/
	
	.syntax unified
	.thumb

/*
 * Context switcher. Save the kernels registers, and load the registers from
 * the process that is running
 */
	.global switch
	.type switch, %function
switch: .fnstart
				push {r0-r12, r14, r15}
/*
 * return_to_user sets the privledge level to unprivledge and switches the
 * the stack pointer to PSP. Call from the kernel to switch back to user mode.
 */
	.global return_to_user
	.type return_to_user, %function
return_to_user: .fnstart
/* Pg. 111, datasheet for exception return codes and behaviour */
/* This code returns to thread mode using floating-point state from PSP, */
/* and execution uses PSP (process stack pointer) after return. */
								ldr pc,=0xFFFFFFED
								.fnend

	.end
	
