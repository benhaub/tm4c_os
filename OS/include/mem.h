#ifndef __MEM_H__
#define __MEM_H__
/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
/* Process stack size. This is independant of the kernel stack which is */
/* created in vectors.s */
#define STACK_SIZE 0x1000 /*4KB Stack */
/* Do not change the value of flash page size. flash memory detection is */
/* based off 2KB page sizes. */
#define FLASH_PAGE_SIZE 0x800

#define NULL (void *)0x0

int get_stackspace(void);
void init_ram(void);

#endif /*__MEM_H__*/