#ifndef __INIT_H__
#define __INIT_H__

/******************************************************************************
* The starting address of the application.  This must be a multiple of 1024		*
* bytes (making it aligned to a page boundary).  A vector table is expected at*
* this location, and the perceived validity of the vector table (stack located*
* in SRAM, reset vector located in flash) is used as an indication of the			*
* validity of the application image.																					*
*																																							*
* The flash image of the boot loader must not be larger than this value.			*
* ****************************************************************************/
#define APP_START_ADDRESS 0x00000000

/******************************************************************************
* The address at which the application locates its exception vector table.		*
* This must be a multiple of 1KB (making it aligned to a page boundary).			*
* Typically, an application will start with its vector table and this value		*
* will default to APP_START_ADDRESS.  This option is provided to cater for		*
* applications which run from external memory which may not be accessible by	*
* the NVIC (the vector table offset register is only 30 bits long).						*
******************************************************************************/
#define VTABLE_START_ADDRESS 0x00000000

/* Stack top is in high memory, making the kernel memory 32KB is size */
#define KERN_STACK 0x00008000

#endif /*__INIT_H__*/
