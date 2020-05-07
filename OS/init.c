/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : init.c                                                          *
 * Synopsis : OS entry point. Boot loader loads the address from the first    *
 *            entry of the vector table, then looks for the entry point in the*
 *            next word                                                       *
 * Date     : May 6th, 2019                                                   *
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>
#include <proc.h>
#include <cstring.h>
#include <types.h>
#include <fs.h>

/* From proc.c */
extern struct pcb ptable[];

int main() {
  int i;
  systick_init();
  led_init();
  while(1) {
    for(i = 0; i < 1000; i++) {
      delay_1ms();
    }
    led_roff();
    for(i = 0; i < 1000; i++) {
      delay_1ms();
    }
    led_ron();
  }
/* Enable all the faults and exceptions. Pg. 173, datasheet */
	NVIC_SYS_HND_CTRL_R |= (1 << 16); /* MEM Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 17); /* BUS Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 18); /* USAGE Enable */
/* Configure Interrupt priorities. Tick exceptions are higher priority */
/* than system calls. SVC is 1 and systick is 0. */
	NVIC_SYS_PRI2_R |= (1 << 29);
	init_ram();
	init_ptable();
	init_context();
	init_fs();
	//start_clocktick();
  uart1_init(B115200);
/* Set up the first user process (the shell) */
	user_init();
	return 0;
}
