/**************************************************************************//**
 * @author	Ben Haubrich
 * @file		syscalls.h
 * @date		July 18th, 2019
 * @details \b Synopsis: \n System calls for tm4c_os kernel services
 *****************************************************************************/
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <types.h>

/* Syscall numbers */
///@def FORK
///Syscall number for fork
#define FORK 0
///@def WAIT
///Syscall number for wait
#define WAIT 1
///@def EXIT
///Syscall number for exit
#define EXIT 2
///@def WRITE
///Syscall number for write
#define WRITE 3
///@def LED
///Syscall number for led
#define LED 4
///@def YEILD
///Syscall number for yield
#define YEILD 5
///@def SPI
///Syscall number for SPI operation
#define SPI 6

/**
 * @enum led_colours
 * @sa led led_states
 * @var led_colours::LED_RED
 *   Set the red colour on the LED
 * @var led_colours::LED_GREEN
 *   Set the green colour on the LED
 * @var led_colours::LED_BLUE
 *   Set the blue colour on the LED
 */   
enum led_colours {LED_RED, LED_GREEN, LED_BLUE};

/**
 * @enum led_states
 * @sa led, led_colours
 * @var LED_OFF
 *   Turn on the LED with the given colour
 * @var LED_ON
 *   Turn on the LED with the given colour
 */
enum led_states {LED_OFF, LED_ON};

/**
 * @enum spi_dir
 * @sa spi
 * @var SPI_TRANSMIT
 *   Transmit SPI data
 * @var SPI_RECEIVE
 *   Receive SPI data
 */
enum spi_dir { SPI_TRANSMIT=0, SPI_RECEIVE };

pid_t fork(void);
int wait(pid_t);
//! @cond Doxygen_Suppress_Warning
/**
 * exit is word aligned because the memory, usage, and bus fault handlers will
 * load the PC with this address if the fault can be services appropriately by
 * the kernel. Loading an unaligned address to the PC has unpredicatable
 * behaviour.
 * @see
 *   Pg. 48 Architecture Reference Manaual
 */
void exit(pid_t) __attribute__((noreturn, naked, aligned(4)));
//! @endcond
int write(char *);
int led(int, int);
void yield();
void spi(int, uint8_t *);

#endif /*__SYSCALLS_H__*/
