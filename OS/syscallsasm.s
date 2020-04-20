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
 * Almost the same as the context switch. Only difference is that we don't 
 * need to update the stack pointer, and we switch to msp (instead of psp)
 * and to privledge (instead of unprivledged). Privledge levels are handled
 * automatically by the exception entry and return mechanisms.
 */
	.global syscall
	.type syscall, %function
syscall: .fnstart
/* Put the program counter and all general purpose registers into the */
/* processes struct context. This is required for fork(). The pc from the */
/* original system call is in the stacked link register. */
					ldr r2, [sp, #4]
					str r2, [r1, #4]
/* Restore stack pointer to where it was before system call. (before the */
/* push {r7, lr} */
/*TODO: This seems a little bit too compiler version dependant. Does GDB have
/* a no-prologue attribute? */
					add r4, sp, #8
					str r4, [r1]
/* The immediate value for svc is not used. The number used for determining */
/* The kernel service is passed through as an argument to syscall() (here */
/* that manifests itself as r0. */
/* The cortex-m4 exception entry and return model take care of context. */
					svc #0
/* Move the returned value from the syscall to r0 so that the kernel */
/* services return the right value */
					ldr r0, [r1, #12]
					bx lr
				.fnend

/*
 * This is the assembly routine for system calls that have one argument.
 */
	.global syscall1
	.type syscall1, %function
syscall1: .fnstart
/* Argument needs to be placed in r1 so svc_handler gets it in arg1. First */
/* save currproc though. */
					mov r3, r1
/* Move the syscalls argument over to r1 and overwrite the currproc pointer. */
					ldr r1, [r2]
/* The immediate value for svc is not used. The number used for determining */
/* The kernel service is passed through as an argument to syscall() (here */
/* that manifests itself as r0. */
					svc #0
/* Move the returned value from the syscall to r0 so that the kernel */
/* services return the right value */
					ldr r0, [r3, #12]
					bx lr
				.fnend

	.end
