/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	init.c
 * Synopsis	:	OS entry point. Boot loader loads the address from the first
 * 						entry of the vector table, then looks for the entry point in the
 * 						next word
 * Date			:	May 6th, 2019
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>
#include <proc.h>
#include <cstring.h>
#include <types.h>

/* From proc.c */
extern struct pcb ptable[];
/* From link.ld */
extern void * _text; extern void * _etext;
word _eflash;

int main() {
/* Enable all the faults and exceptions. Pg. 173, datasheet */
	NVIC_SYS_HND_CTRL_R |= (1 << 16); /* MEM Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 17); /* BUS Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 18); /* USAGE Enable */
/* Find out how much flash we need to protect */
	_eflash = (word)(&_etext) - (word)(&_text);
/* Write protect flash memory that contains kernel code. Pg. 578, datasheet. */
	protect_flash(_eflash);
/* Configure Interrupt priorities. Tick exceptions are higher priority */
/* than system calls. SVC is 1 and systick is 0. */
	NVIC_SYS_PRI2_R |= (1 << 29);
	init_ram();
/* Get the process table ready for scheduling. */
	init_ptable();
	init_context();
/* Set up the first user process (the shell) */
	start_clocktick();
	user_init();
	return 0;
}
