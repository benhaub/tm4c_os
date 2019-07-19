/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.c
 * Synopsis	:	Context switcher just for system calls. Always switches to kernel
						: context from user context.
 * Date			:	July 18th, 2019
 *****************************************************************************/
	.syntax unified
	.thumb

/*
 * Almost the same as switch. Only difference is that we don't need to update
 * the stack pointer, and we switch to msp (instead of psp) and to privledge
 * (instead of unprivledged). By using r7, system calls support a maximum of
 * 7 arguments in one call.
 */
	.global syscall
	.type syscall, %function
syscall: .fnstart
					push {r0-r12, r14}
					mrs r7, CONTROL
					bic r7, r7, #0x3
					msr CONTROL, r7 /* Now privledged and using msp */
/* The immediate value for svc is not used. The number used for determining */
/* The kernel service is passed through as an argument to syscall() (here */
/* that manifests itself as r0. */
					svc #0
					pop {r0-r12, r14}
					bx lr
				.fnend

	.end
