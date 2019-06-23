/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	init.c
 * Synopsis	:	OS entry point. Boot loader loads the address from the first
 * 						entry of the vector table, then looks for the entry point in the
 * 						next word
 * Date			:	May 6th, 2019
 *****************************************************************************/
#include <init.h>
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <sched.h>
#include <mem.h>
#include <proc.h>
#include <string.h>

/* From sched.c */
extern struct pcb ptable[];

int main() {
/* Secure a page for the kernel. The memory is already being used, but */
/* Hasn't been protected yet. This is a permanent change so no other */
/* flash memory regions will be protected using this register. */
	FLASH_FMPPE0_R &= ~(1 << 31);
	init_ram();
/* Get the process table ready for scheduling. */
	init_ptable();
/* Set up the kernel process in the ptable. Do not touch the context since */
/* The kernel will save it's registers before switching. */
	ptable[0].state = RUNNING;
	strncpy("kernel", ptable[0].name, 6);
	ptable[0].pid = 0;
	led_init();
	led_ron();
	delay_1ms();
	led_roff();
	led_gron();
	return 0;
}
