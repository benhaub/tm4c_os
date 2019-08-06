/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	kernel_services.h
 * Synopsis	:	Supported kernel services for users.
 * Date			:	July 18th, 2019
 *****************************************************************************/
#ifndef __KERNELSERVICES_H__
#define __KERNELSERVICES_H__

int sysfork(void);
int syswait(int);
int sysexit();

#endif /*__KERNELSERVICES_H__*/
