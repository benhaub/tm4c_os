#ifndef __MEM_H__
#define __MEM_H__
/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
#include <types.h>
#include <proc.h>

/* From link.ld */
extern void *smainsize;
/* From initshell. Not called. Only used to calculate ksize. */
extern int smain(void);

/* Process stack size. This is independant of the kernel stack which is */
/* created in vectors.s at the top of the vector table. */
#define STACK_SIZE 0x400 /*1KB Stack */
/* Do not change the value of flash page size. flash memory protection is */
/* based off 2KB page sizes. */
#define FLASH_PAGE_SIZE 0x800
/* Beginning of SRAM */
#define _SRAM 0x20000000
/* End of SRAM */
#define SRAM_ 0x20008000
/* Beginning of FLASH */
#define _FLASH 0x0
/* End of FLASH */
#define FLASH_ 0x00040000
/* Number of flash pages. */
#define FLASH_PAGES FLASH_ / FLASH_PAGE_SIZE
/* Number of SRAM Pages. */
#define SRAM_PAGES (SRAM_ - _SRAM) / STACK_SIZE
/* 1 KB */
#define KB 1024u
/* 1 MB */
#define MB 1024u*KB
/* Size of the kernel in flash */
#define KSIZE (word)((word)smain + (word)&smainsize)
/* Number of flash pages used by the kernel */
#define KFLASHPGS ((KSIZE / FLASH_PAGE_SIZE) + 1)

int get_stackspace(void);
void free_stackspace(int);
void init_ram(void);

#endif /*__MEM_H__*/
