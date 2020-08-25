/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.h
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __KERNELSERVICES_H__
#define __KERNELSERVICES_H__

int sysflash(void *, void *, void *);
int sysfork(void);
int syswait(int);
int sysexit(int);
int syswrite(const char *);
void printk(const char *, ...);
int sysled(int, int);

#endif /*__KERNELSERVICES_H__*/
