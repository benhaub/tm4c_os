/******************************************************************************
 *Authour : Ben Haubrich                                                      *
 *File    : vectors.s                                                         *
 *Synopsis: vector table, fault & reset handlers for ARM Cortex M-series      *
 *Date    : May 6th, 2019                                                     *
 *****************************************************************************/
/* Please refer to "Using as - The GNU Assembler" for all syntax. Only the
/* opcodes (ldr, mov, str) and the operands they accept are ARM specific. */
/* Some machine configurations provide additional directives such as  */
/* .thumb and .syntax for ARM. */
	.syntax unified
	.thumb

/* If you change the stack size, make sure to adjust the MAX_PROC define in */
/* mem.h */
	.section .stack, "wx"
	.align 4
STACK_TOP:
	.skip 0x800, 0x0
	
	.data
/* Kernel Ram usage. */
	.global KRAM_USE
	.set KRAM_USE, Vectors
/* Reset_EXCP needs to be global so it can be used as an entry point. */
	.global Reset_EXCP
/* processor_state is externed in handlers.c for svc calls. */
	.global processor_state
/* A way to dance around and deal with exception mechanisms */
	.global kernel_entry

	.section .intvecs

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {STACK_TOP, Reset_EXCP,...,SYST_EXCP}; */

/* Note that this is the KERNEL stack pointer not a user stack pointer. */
/* They are unrelated. The kernel can have a different sized stack than a */
/* user. */
Vectors:
	.word STACK_TOP + 0x800 /* Boot loader gets kernel stack pointer from here*/
	.word Reset_EXCP	/* Reset Exception */
	.word NMI_EXCP		/* Non-maskable interrupt */
	.word HFAULT			/* Hard Fault */
	.word MM_FAULT		/* Memory Management Fault*/
	.word BFAULT			/* Bus Fault */
	.word UFAULT			/* Usage Fault */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word SVC_EXCP		/* Supervisor Call */
	.word DM_EXCP			/* Debug Monitor */
	.word 0						/* Reserved Space */
	.word PSV_EXCP		/* PendSV */
	.word SYST_EXCP		/* SysTick */

	.text

	.align 2
	.type Reset_EXCP, %function
Reset_EXCP: .fnstart
/* Calculate how much ram the kernel is using so we know where to start */
/* allocating ram pages for user programs. See get_stackspace(). */
/* Where ever the top of stack is determines how much space the kernel is */
/* using. The end of the kernel is the stack top because of how the linker */
/* script use SRAM for for the kernel. */
						mov r0, #0x400 /* Stack Size */
						mov r1, sp
						sub r1, r1, #0x20000000
/* Divide this by the current position of the sp to get an integer for */
/* get_stackspace. */
						udiv r1, r0
/* "push" the value onto the first spot in the stack marked by the symbol */
/* KRAM_USAGE. */
						push {r1}
/* Enable the FPU. Taken from tivaware bootloader code, but also detailed */
/* on page 74 of the Cortext-M4 TRM. */
            movw    r0, #0xED88
            movt    r0, #0xE000
            ldr     r1, [r0]
            orr     r1, r1, #0x00F00000
            str     r1, [r0]
						b main
						.fnend

	.align 2
	.type NMI_EXCP, %function
NMI_EXCP: .fnstart
				 b nmi_handler
				 .fnend

	.align 2
	.type HFAULT, %function
HFAULT: .fnstart
				b hfault_handler
				.fnend

	.align 2
	.type MM_FAULT, %function
MM_FAULT: .fnstart
					b mm_handler
					.fnend

	.align 2
	.type BFAULT, %function
BFAULT: .fnstart
        b b_handler
				.fnend

	.align 2
	.type UFAULT, %function
UFAULT:	.fnstart
				b u_handler
				.fnend

	.align 2
	.type SVC_EXCP, %function
SVC_EXCP: .fnstart
				 b svc_handler
				 .fnend

	.align 2
	.type DM_EXCP, %function
DM_EXCP: .fnstart
				b dm_handler
				.fnend

	.align 2
	.type PSV_EXCP, %function
PSV_EXCP: .fnstart
				 b psv_handler
				 .fnend

/*
 * The reason we don't do a direct branch to the handler is to avoid context
 * switching while in handler mode. The processor's exception mechanism makes
 * switching in handler mode very difficult. The exception mechanism exits
 * handler mode on the bx lr, and stack saving happens in kernel_entry. The
 * purpose of this code here is too save r0, pc and lr because it will be
 * changed when we enter the exeption handler c code. kernel_entry will need
 * these values in order to save the correct context.
 */
	.align 2
	.type SYST_EXCP, %function
SYST_EXCP: .fnstart
/* Get the processes stack pointer and save it */
					mrs r0, psp
/* Save the value of the exception stack r0. */
					mov r4, r0
/* Save the value of the exception stack pc. */
					ldr r5, [r0, #24]
/* Save the value of the exception stack lr. */
					ldr r6, [r0, #20]
/* Make sure r5 has bit one set for thumb instructions. */
					ands r8, r5, #0x1
					bne Thumb
					add r5, r5, #0x1
Thumb:
					ldr r3,=syst_handler
/* overwrite r0 on the stack for a function call. It will be returned in */
/* kernel_entry(). */
					str r0, [r4]
/* Place syst_handler on the stacked pc. Exception mechanism retores it to lr*/
					str r3, [r0, #24]
/* Save r7 in case syst_handler changes it. */
					mov r8, r7
/* Exception return mechanism will return r0-r3 to pre-exception values. */
/* r4, r5, and r6 remain unscathed. */
/* Change thread mode privledge level to privledged. */
					mrs r3, CONTROL
					bic r3, r3, #0x1
					msr CONTROL, r3
					bx lr
					.fnend

/*
 * Entry to the kernel from the systick isr. It is executed from thread
 * mode so that it is ossible to save stacks and context switch properly.
 * It makes sure the process stack is returned to it's orginal state, and then
 * pushes the context onto the stack and saves the stack pointer (psp) before
 * switching stacks from psp to msp. The corresponding pop is made from swtch
 * so make sure that the push here and pop there are consistent.
 */
	.align 2
	.type kernel_entry, %function
kernel_entry: .fnstart
/* Transfer r0 to r9 so that r0 can be returned to it's saved pre-systick */
/* interrupt value. */
							mov r9, r0
/* Return r0 to it's initial value */
							mov r0, r4
/* Save the lr before moving the saved pc from systick isr into it. */
							mov r4, r14
							mov r14, r6
							push {r0-r3, r5, r7, r12, r14}
/* Save the stack pointer to context struct */
							str sp, [r9] 
/* Switch stacks to msp and restore lr to it's original value */
							mov r14, r4
							mrs r3, CONTROL
							bic r3, r3, #0x2
							msr CONTROL, r3
							bx lr
							.fnend

/* Change the processor state to either enable or disable interrupts. */
/* use 1 as a parameter to enable, 0 to disable. */
	.align 2
	.type processor_state, %function
processor_state: .fnstart
								 cmp r0, #0
								 beq Disable
								 cpsie i
								 b Return
Disable:				 cpsid i
Return:					 bx lr
								 .fnend
	.end
