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

/* Pg.116 & 19, ALT - These routines will be used from outside this file */ 
	.data
		.global SVC_ISR, MM_FAULT, OS_ENTRY 

/* Pg.19, ALT - user-named sections created with the .sect assembler directive*/
/* are initialized. */
	.sect "intvecs"

/*
 * Pg.121, ALT - .word is a directive that initializes 32-bit integers. I will
 * define these words as relocatble symbols which will be refered to in a .c
 * file later on. When complied, the symbols will be given an address value.
 */

/* Pg.103, datasheet - Table 2-8 details the vector table. */

Vectors:												/* Description */							
	.word OS_ENTRY								/* The boot loader jumps to OS from here */
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
	.word GPIOA_ISR
	.word GPIOB_ISR
	.word GPIOC_ISR
	.word GPIOD_ISR
	.word GPIOE_ISR
	.word UART0_ISR
	.word UART1_ISR
	.word SSIO_ISR								/* Synchronous Serial Interface */
	.word I2C0_ISR								/* Inter-intergrated circuit */
	.word PWM0F_ISR								/* Pulse Width Modulator Fault */
	.word PWM0G0_ISR							/* Pulse Width Modulator Generator 0 */
	.word	PWM0G1_ISR
	.word PWM0G2_ISR
	.word QEI0_ISR								/* Quadreture Encoder Interface */
	.word ADC0S0_ISR							/* Analog to Digital Converter Sequence 0 */
	.word	ADC0S1_ISR
	.word ADC0S2_ISR
	.word ADC0S3_ISR
	.word WT01_ISR								/* Watchdog timer 0 and 1 */
	.word TIMER0A_ISR
	.word TIMER0B_ISR
	.word TIMER1A_ISR
	.word	TIMER1B_ISR
	.word TIMER2A_ISR
	.word TIMER2B_ISR
	.word AC0_ISR									/* Analog Comparator 0 */
	.word	AC1_ISR
	.word 0												/* Reserved */
	.word SYSCTL_ISR							/* System Control */
	.word FMCEC_ISR								/* Flash Memory Control and EEPROM Control */
	.word	GPIOF_ISR
	.word	0
	.word	0
	.word	UART2_ISR
	.word	SSI1_ISR
	.word TIMER3A_ISR
	.word TIMER3B_ISR
	.word	I2C1_ISR
	.word	QEI1_ISR
	.word	CAN0_ISR								/* Controller Area Network 0 */
	.word	CAN1_ISR
	.word	0
	.word	0
	.word	HM_ISR									/* Hibernation Module */
	.word	USB_ISR
	.word PWMG3_ISR
	.word UDMAS_ISR								/* Micro Direct Memory Access Software*/
	.word	UDMAE_ISR								/* Micro Direct Memory Access Error */
	.word	ADC1S0_ISR
	.word ADC1S1_ISR
	.word	ADC1S2_ISR
	.word ADC1S3_ISR
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SSI2_ISR
	.word SSI3_ISR
	.word UART3_ISR
	.word	UART4_ISR
	.word	UART5_ISR
	.word	UART6_ISR
	.word UART7_ISR
	.word	0
	.word	0
	.word	0
	.word	0
	.word	I2C2_ISR
	.word I2C3_ISR
	.word	TIMER4A_ISR
	.word	TIMER4B_ISR
	.word	0
	.word	0
	.word 0
	.word 0
	.word 0
	.word	0
	.word	0
	.word	0
	.word 0
	.word 0
	.word 0
	.word	0
	.word	0
	.word	0
	.word 0
	.word 0
	.word 0
	.word	0
	.word	0
	.word	0
	.word TIMER5A_ISR
	.word TIMER5B_ISR
	.word	TIMER0A_ISR
	.word	TIMER0B_ISR
	.word	TIMER1A_ISR
	.word TIMER1B_ISR
	.word TIMER2A_ISR
	.word	TIMER2B_ISR
	.word	TIMER3A_ISR
	.word	TIMER3B_ISR
	.word TIMER4A_ISR
	.word TIMER5A_ISR
	.word TIMER5B_ISR
	.word SYI_ISR									/* System Exception (Imprecise) */
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word PWM1G0_ISR
	.word PWM1G1_ISR
	.word PWM1G2_ISR
	.word PWM1G3_ISR
	.word PWM1F_ISR

	/* .text section is for executable code */
	.text


	/*
	 * These faults are entered when something in the cpu goes wrong during
	 * execution. They can't really be handled, but must be entered in the event
	 * that they occur. Read more about these faults on Pg. 432, ARmanual
	 */

	/* Pg.94, ALT */
HFAULT: .fnstart
				hloop:
					b hloop
				.fnend

BFAULT: .fnstart
				bloop:
					b bloop
				.fnend

UFAULT:	.fnstart
				uloop:
					b uloop
				.fnend
/* Reset Exception Handler. See Pg.432 for reset conditions */
/* Reset jumps back to the OS init function. */
RESET_ISR: .fnstart
	b init

	/* Pg.111, ALT */
	.end
