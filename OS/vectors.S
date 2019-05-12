/**************************************************************************
 *Authour	:	Ben Haubrich																									*
 *File		:	vectors.S																											*
 *Synopsis:	vector table, fault & reset handlers for ARM Cortex M-series	*
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
 * Descriptions of each field can be found on Pg.49 also.
 */

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {OS_ENTRY, Reset_EXCP,...,SYST_ISR}; */
	.section "intvecs"

Vectors:
	.word start										/* The boot loader jumps to OS from here */
	.word Reset_EXCP							/* Reset Exception */
	.word NMI_ISR									/* Non-maskable interrupt */
	.word HFAULT									/* Hard Fault */
	.word MM_FAULT								/* Memory Management Fault*/
	.word BFAULT									/* Bus Fault */
	.word UFAULT									/* Usage Fault */
	.word 0												/* Reserved Space */
	.word 0												/* Reserved Space */
	.word 0												/* Reserved Space */
	.word 0												/* Reserved Space */
	.word SVC_ISR									/* Supervisor Call */
	.word DM_ISR									/* Debug Monitor */
	.word 0												/* Reserved Space */
	.word PSV_ISR									/* PendSV */
	.word SYST_ISR								/* SysTick */
/*
 * These faults are entered when something in the cpu goes wrong during
 * execution. They can't really be handled, but must be entered in the event
 * that they occur. Read more about these faults on Pg. 432, ARmanual
 */
	.text
	.global Vectors
	.global Reset_EXCP
/* Align 2 because we are compiling thumb instructions with */

/* Reset Exception Handler. See Pg.432 for reset conditions */
/* Reset jumps back to the OS init function. */
	.align 2
	.type Reset_EXCP, %function
Reset_EXCP: .fnstart
start:
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
