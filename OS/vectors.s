/**************************************************************************
 *Authour	:	Ben Haubrich																									*
 *File		:	vectors.s																											*
 *Synopsis:	vector table, fault & reset handlers for ARM Cortex M-series	*
 *Date		: May 6th, 2019																									*
 **************************************************************************/
/* Please refer to "Using as - The GNU Assembler" for all syntax. Only the
/* opcodes (ldr, mov, str) and the operands they accept are ARM specific. */
/* Some machine configurations provide additional directives such as  */
/* .thumb and .syntax for ARM. */
	.syntax unified
	.thumb

	.section .stack, "wx"
	.align 4
STACK_TOP:
	.skip 0x1000, 0x0
	
	.data
/* Reset_EXCP needs to be global so it can be used as an entry point. */
	.global Reset_EXCP
/* processor_state is externed in handlers.c for svc calls. */
	.global processor_state
/* Kernel Ram usage. */
KRAM_USE:
	.global KRAM_USE

	.section .intvecs

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {STACK_TOP, Reset_EXCP,...,SYST_ISR}; */

/*
 * The STACK_TOP expression says: Take the address of STACK_TOP, move it by
 * 4KB. Then at the STACK_TOP label above, it says, now give me 4KB of space
 * for this address.
 */
Vectors:
	.word STACK_TOP + 0x1000 /* Boot loader gets kernel stack pointer from here*/
	.word Reset_EXCP	/* Reset Exception */
	.word NMI_ISR			/* Non-maskable interrupt */
	.word HFAULT			/* Hard Fault */
	.word MM_FAULT		/* Memory Management Fault*/
	.word BFAULT			/* Bus Fault */
	.word UFAULT			/* Usage Fault */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word SVC_ISR			/* Supervisor Call */
	.word DM_ISR			/* Debug Monitor */
	.word 0						/* Reserved Space */
	.word PSV_ISR			/* PendSV */
	.word SYST_ISR		/* SysTick */

	.text

	.align 2
	.type Reset_EXCP, %function
Reset_EXCP: .fnstart
/* Calculate how much ram the kernel is using so we know where to start */
/* allocating ram pages for user programs. See get_stackspace(). */
/* Where ever the top of stack is determines how much space the kernel is */
/* using. */
/* 32KB of ram in r0. */
						mov r0, #0x8000
						mov r1, sp
						sub r1, r1, #0x20000000
/* Divide this by the current position of the sp to get an integer for */
/* get_stackspace. */
						udiv r0, r1
						ldr r3,=KRAM_USE
						str r0, [r3]
						b main
						.fnend

	.align 2
	.type NMI_ISR, %function
NMI_ISR: .fnstart
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
	.type SVC_ISR, %function
SVC_ISR: .fnstart
				 b svc_handler
				 .fnend

	.align 2
	.type DM_ISR, %function
DM_ISR: .fnstart
				b dm_handler
				.fnend

	.align 2
	.type PSV_ISR, %function
PSV_ISR: .fnstart
				 b psv_handler
				 .fnend

	.align 2
	.type SYST_ISR, %function
SYST_ISR: .fnstart
					b syst_handler
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
/* Pg.111, ALT */
	.end
