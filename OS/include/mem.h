/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : proc.h                                                          *
 * Synopsis : Process related structs and functions                           *
 * Date     : June 5th, 2019                                                  *
 *****************************************************************************/
#ifndef __MEM_H__
#define __MEM_H__

#include <types.h>
#include <proc.h>

/* From link.ld */
extern void *smainsize;
/* From initshell. Not called. Only used to calculate ksize. */
extern int smain(void);
/* From vectors.s */
extern const int KRAM_USE;

/**
 * Process stack size. This is independant of the kernel stack which is
 * created in vectors.s at the top of the vector table. If you change this
 * value, also change the stack size used in Reset_EXCP.
 */
#define STACK_SIZE 0x400 /*1KB Stack */
/* Context switch stack size (in bytes, 4 bytes per uint32_t saved) */
#define CTXSTACK 36
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
/* Size of the kernel in flash. This calculation is possible because */
/* .text.smain is the last of the code place in flash (see link.ld). */
#define KSIZE (uint32_t)((uint32_t)smain + (uint32_t)&smainsize)
/* Number of flash pages used by the kernel */
#define KFLASHPGS ((KSIZE / FLASH_PAGE_SIZE) + 1)

/* The top of stack for any process given the ram page, x. */
#define stacktop(x) (x ? _SRAM + x*STACK_SIZE + STACK_SIZE - 4 : _SRAM + STACK_SIZE - 4)
/* The bottom of stack for any process given the ram page, x. */
#define stackbottom(x) (x ? _SRAM + x*STACK_SIZE : 0)

int get_stackpage(void);
void free_stackpage(int);
void init_ram(void);
void mpu_tm4cOS_init(void);
void create_user_memory_region(int);

#endif /*__MEM_H__*/
