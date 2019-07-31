/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	syscalls.h
 * Synopsis	:	System calls for tm4c_os kernel services
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

int fork(void);
int wait(int);
int procexit(int);

#endif /*__SYSCALLS_H__*/
