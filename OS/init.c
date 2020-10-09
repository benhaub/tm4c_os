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
#include <kernel_services.h>

/* From proc.c */
extern struct pcb ptable[];

int main() {
/* Enable all the faults and exceptions. Pg.173, datasheet */
	NVIC_SYS_HND_CTRL_R |= (1 << 16); /* MEM Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 17); /* BUS Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 18); /* USAGE Enable */
  if(-1 == uart1_init(B115200)) {
    syswrite("Failed to start UART\n\nr");
  }
  /* We already in the kernel so we can use the kernel services directly. */
  syswrite("Initialising tm4c_os\n\r");
	led_init();
  if(-1 == ssi0_init_master(0,0x7,2, 0)) {
    syswrite("Failed to start SSI0\n\r");
  }
/* Configure Interrupt priorities. SVC exceptions are higher priority */
/* than tick interrupts. SVC is 0 and systick is 1. */
	NVIC_SYS_PRI3_R |= (1 << 29);
	init_ram();
  mpu_tm4cOS_init();
	init_ptable();
	start_clocktick();
/* Set up the first user process (the shell) */
	user_init();
	return 0;
}
