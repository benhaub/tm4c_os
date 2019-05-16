/**************************************************************************
 *Authour	:	Ben Haubrich																									*
 *File		:	vectors.S																											*
 *Synopsis:	vector table, fault & reset handlers for ARM Cortex M-series	*
 *Date		: May 6th, 2019																									*
 **************************************************************************/

/******************************************************************
 * Sources Used:																									*
 *	ARM Assembly Language Tools v18.1.0.LTS Users Guide (Rev. U)	*
 *	- Refer to as ALT																							*
 *	Tiva C Series TM4C123GH6PM Microcontroller Data Sheet (Rev .E)*
 *	- Refer to as datasheet																				*
 *	ARMv7 Architecture Reference Manual														*
 *	- Refer to as ARmanual																				*
 ******************************************************************/
/* 
 * Pg.49, ALT - General syntax for each line of arm assembly
 * 	[label[:]]mnemonic [operand list][;comment]
 * Descriptions of each field can be found on Pg.49 also. It's important to
 * note that some of the syntax (like directives) are understood by the GNU
 * assembler, while instructions like branching and loading registers are
 * understood by the architecture. Arm assembly guides won't tell you much
 * about things like .section and .fnstart.
 */

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {OS_ENTRY, Reset_EXCP,...,SYST_ISR}; */
	.syntax unified
	.thumb
	
	.bss
STACK_TOP:
	.skip 0x1000, 0

	.text
	.global Reset_EXCP

	.section .intvecs
/* 
 * See the linkerscript for memory layout. Placing the stack top here gives
 * the kernel 32KB of space.
 */

/*
 * The STACK_TOP expressions says: Take the address of STACK_TOP, move it by
 * 32KB. Then at the STACK_TOP label above, it says, now give me 4KB of space
 * for this address.
 */
Vectors:
	.word STACK_TOP + 0x8000	/* Boot loader gets kernel stack pointer from here*/
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

/*
 * These faults are entered when something in the cpu goes wrong during
 * execution. They can't really be handled, but must be entered in the event
 * that they occur. Read more about these faults on Pg. 432, ARmanual
 */

/* Align 2 because we are compiling thumb instructions with */

/* Reset Exception Handler. See Pg.432 for reset conditions */
/* Reset jumps back to the OS init function. */
	.text

	.align 2
	.type Reset_EXCP, %function
Reset_EXCP: .fnstart
						b main
						.fnend

	.align 2
	.type NMI_ISR, %function
NMI_ISR: .fnstart
				 b .
				 .fnend

	.align 2
	.type HFAULT, %function
HFAULT: .fnstart
				b .
				.fnend

	.align 2
	.type MM_FAULT, %function
MM_FAULT: .fnstart
					b .
					.fnend

	.align 2
	.type BFAULT, %function
BFAULT: .fnstart
				bloop:
					b bloop
				.fnend
	.align 2
	.type UFAULT, %function
UFAULT:	.fnstart
				uloop:
					b uloop
				.fnend

	.align 2
	.type SVC_ISR, %function
SVC_ISR: .fnstart
				 b .
				 .fnend

	.align 2
	.type DM_ISR, %function
DM_ISR: .fnstart
				b .
				.fnend

	.align 2
	.type PSV_ISR, %function
PSV_ISR: .fnstart
				 b .
				 .fnend

	.align 2
	.type SYST_ISR, %function
SYST_ISR: .fnstart
					b .
					.fnend
/* Pg.111, ALT */
	.end
