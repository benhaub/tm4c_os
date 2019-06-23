/**************************************************************************
 *Authour	:	Ben Haubrich																									*
 *File		:	vectors.S																											*
 *Synopsis:	vector table, fault & reset handlers for ARM Cortex M-series	*
 *Date		: May 6th, 2019																									*
 **************************************************************************/
	.syntax unified
	.thumb

	.section .stack, "wx"
	.align 4
/* Might use address 0 as NULL. */
STACK_TOP:
	.skip 0x1000, 0x0
	
	.data
/* Needs to be global so it can be used as an entry point. */
	.global Reset_EXCP

	.section .intvecs

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {STACK_TOP, Reset_EXCP,...,SYST_ISR}; */

/*
 * The STACK_TOP expressions says: Take the address of STACK_TOP, move it by
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
/* Pg.111, ALT */
	.end
