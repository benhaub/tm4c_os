/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.h
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <types.h>

/* Syscall numbers */
#define FORK 0
#define WAIT 1
#define EXIT 2
#define WRITE 3
#define LED 4

enum led_colours {LED_RED, LED_GREEN, LED_BLUE};
enum led_states {LED_OFF, LED_ON};

int fork(void);
int wait(pid_t);
/**
 * exit is word aligned because the memory, usage, and bus fault handlers will
 * load the PC with this address if the fault can be services appropriately by
 * the kernel. Loading an unaligned address to the PC has unpredicatable
 * behaviour.
 * @see
 *   Pg. 48 Architecture Reference Manaual
 */
int exit(pid_t) __attribute__((noreturn, aligned(4)));
int write(char *);
int led(int, int);

#endif /*__SYSCALLS_H__*/
