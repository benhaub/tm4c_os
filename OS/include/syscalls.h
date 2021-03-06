/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.h
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

int flash(void *, void *, void *);
int fork(void);
int wait(int);
int exit(int) __attribute__((noreturn));

#endif /*__SYSCALLS_H__*/
