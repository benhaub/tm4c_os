/**************************************************************************//**
 * @author   Ben Haubrich                                                    
 * @file     main.c                                                          
 * @date     May 6th, 2019                                                   
 * @details \b Synopsis: \n OS entry point. Boot loader loads the address from
 *           the first entry of the vector table, then looks for the entry point
 *           in the next uint32_t                                             
 * @mainpage TM4C OS
 * @section Introduction
 *   TM4C OS is an simple operating system with the intent of demonstrating the
 *   core functionality of an operating system kernel.\n
 *   Much of the code is naively written on purpose (especially device drivers)
 *   to help users focus more on the purpose rather than optimization techniques
 *   which often lead to having to understand serveral concepts in tandem and
 *   more complicated code.
 * @section Compiling
 *   Run make with no arguments. The makefile will produce a binary file located
 *   in the .binary folder.
 * @section Flashing
 * @section Debugging
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>
#include <proc.h>
#include <cstring.h>
#include <kernel_services.h>

int init() {
/* Enable all the faults and exceptions. Pg.173, datasheet */
	NVIC_SYS_HND_CTRL_R |= (1 << 16); /* MEM Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 17); /* BUS Enable */
	NVIC_SYS_HND_CTRL_R |= (1 << 18); /* USAGE Enable */
  NVIC_CFG_CTRL_R |= 1;
  NVIC_CFG_CTRL_R |= (1 << 4); /* Enable traps on division by zero. */
/* Configure Interrupt priorities. SVC exceptions are higher priority than */
/* SysTick so that the kernel can't be interrupted. Pg. 170, datasheet. */
	NVIC_SYS_PRI3_R |= (3 << 29); //SysTick
	NVIC_SYS_PRI2_R |= (2 << 29); //SVC
	NVIC_SYS_PRI1_R |= (3 << 21); //Usage
	NVIC_SYS_PRI1_R |= (2 << 5); //Mem
  struct clocksource_config_t cs_config = {.oscsrc = 0x0, //Main Osciallator
                                           .use_pll = 0, //Not using PLL
                                           .sysdiv = 0, //No divison.
                                           .sysdiv2 = 0,
                                           .div400 = 0,
                                           .sysdiv2lsb = 1}; //Don't care
	#pragma GCC diagnostic push //Remember the diagnostic state
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers" //choose to ignore
  if(-1 == set_clocksource(cs_config, &SysClkFrequency)) {
    return 0;
  }
	#pragma GCC diagnostic push //push the state back to before we ignored
  if(-1 == uart1_init(115200u)) {
    return 0;
  }
  /* We are already in the kernel so we can use the kernel services directly. */
  syswrite("Initialising tm4c_os\n\r");
	led_init();
  if(-1 == ssi0_init_master(0,0x7,2, 0)) {
    syswrite("Failed to start SSI0\n\r");
  }
	init_ram();
  mpu_tm4cOS_init();
	init_ptable();
	start_clocktick(1, 10);
/* Set up the first user process (the shell) */
	user_init();
	return 0;
}
