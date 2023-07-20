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
 * @section Compiling
 *   Run make with no arguments. The makefile will produce a binary file located
 *   in the .binary folder.
 * @section Flashing
 * @section Debugging
 *****************************************************************************/
#include "hw.h"
#include "mem.h"
#include "proc.h"
#include "cstring.h"
#include "tm4c123gh6pm.h"
#include "kernel_services.h"

#include "sysctl.h"

/*
 * Hardware peripheral initialization for custom applications.
 */
static void appInit() {
  ledInit();
  gptmTimerInit();
  gpioInit(4);

  //SSI clock divisor must not be lower than 10.
  if(-1 == ssi0InitMaster(10))
    syswrite("Failed to start SSI0\n\r");
}

/*
 * @brief
 *   Initialization of required kernel software
 */
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
  NVIC_SYS_PRI1_R |= (2 << 5);  //Mem

  SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  if(-1 == uart1Init(115200u)) {
    syswrite("Failed to initialize UART1\n\r");
  }

  /* We are already in the kernel so we can use the kernel services directly. */
  syswrite("Initialising tm4c_os\n\r");
  init_ram();
  startClocktick(1, 10);
  mpuInit();

  appInit();
  user_init();

/* Never reached */
  return 0;
}
