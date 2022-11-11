/**************************************************************************//**
 * @author	Ben Haubrich
 * @file		kernel_services.h
 * @date		July 18th, 2019
 * @details \b Synopsis: \n Supported kernel services for users.
 *****************************************************************************/
#ifndef __KERNELSERVICES_H__
#define __KERNELSERVICES_H__

pid_t sysfork(void);
int syswait(int);
void sysexit(int);
int syswrite(const char *);
void printk(const char *, ...);
int sysled(int, int);
void sysyield();
void sysspi(int, uint8_t *);

#endif /*__KERNELSERVICES_H__*/
