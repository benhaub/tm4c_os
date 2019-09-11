#ifndef __MEM_H__
#define __MEM_H__
/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	proc.h
 * Synopsis	:	Process related structs and functions
 * Date			:	June 5th, 2019
 *****************************************************************************/
/* Process stack size. This is independant of the kernel stack which is */
/* created in vectors.s at the top of the vector table. If you change this */
/* value, also change the number in r0 of Reset_EXCP for the calculation of */
/* kernel ram usage. */
#define STACK_SIZE 0x400 /*1KB Stack */
/* Do not change the value of flash page size. flash memory detection is */
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

int get_stackspace(void);
int free_stackspace(int);
void init_ram(void);

#endif /*__MEM_H__*/
