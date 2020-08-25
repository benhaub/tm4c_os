/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.h
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

enum led_colours {LED_RED, LED_GREEN, LED_BLUE};
enum led_states {LED_OFF, LED_ON};

int flash(void *, void *, void *);
int fork(void);
int wait(int);
int exit(int) __attribute__((noreturn));
int write(char *);
int led(int, int);

#endif /*__SYSCALLS_H__*/
