/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    mem.h                                                         
 * @date    June 5th, 2019                                                  
 * @details \b Synopsis: \n Process related structs and functions                           
 *****************************************************************************/
#ifndef __MEM_H__
#define __MEM_H__

#include <types.h>
#include <proc.h>

//! @cond Doxygen_Suppress_Warning
/* From link.ld. See linkerscript. */
extern void *mainsize;
/* From userinit. Not called. Only used to calculate ksize. */
extern int main(void);
/* From vectors.s */
extern const int KRAM_USE;
//! @endcond

/**
 * @def STACK_SIZE
 *   Process stack size. This is independant of the kernel stack which is
 *   created in vectors.s at the top of the vector table. If you change this
 *   value, also change the stack size used in Reset_EXCP.
 */
#define STACK_SIZE 0x400 /*1KB Stack */
///@def CTXSTACK
///  Context switch stack size (in bytes, 4 bytes per uint32_t saved)
#define CTXSTACK 36
///@def FLASH_PAGE_SIZE
///  The size of one memory chunk of flash
#define FLASH_PAGE_SIZE 0x800
///@def _SRAM
///  Beginning of SRAM
#define _SRAM 0x20000000
///@def SRAM_
///  End of SRAM
#define SRAM_ 0x20008000
///@def _FLASH
///  Beginning of FLASH
#define _FLASH 0x0
///@def FLASH_
///  End of FLASH
#define FLASH_ 0x00040000
///@def FLASH_PAGES
///  Number of flash pages.
#define FLASH_PAGES FLASH_ / FLASH_PAGE_SIZE
///@def SRAM_PAGES
///  Number of SRAM Pages.
#define SRAM_PAGES (SRAM_ - _SRAM) / STACK_SIZE
///@def KB
///  1KB
#define KB 1024u
///@def MB
///  1MB
#define MB 1024u*KB
///@def KSIZE
///  Size of the kernel in flash. This calculation is possible because
///  .text.main is the last of the code place in flash (see link.ld).
#define KSIZE (uint32_t)((uint32_t)main + (uint32_t)&mainsize)
///@def KFLASHPGS
///  Number of flash pages used by the kernel
#define KFLASHPGS ((KSIZE / FLASH_PAGE_SIZE) + 1)

///@def stacktop()
///  The top of stack for any process given the ram page, x.
///@param x
///  The rampg to calculate the stack top for
#define stacktop(x) (x ? _SRAM + x*STACK_SIZE + STACK_SIZE - 4 : _SRAM + STACK_SIZE - 4)
///@def stackbottom()
///  The bottom of stack for any process given the ram page, x.
///@param x
///  The rampg to calculate the stack bottom for
#define stackbottom(x) (x ? _SRAM + x*STACK_SIZE : 0)

int get_stackpage(void);
void free_stackpage(int);
void init_ram(void);
void mpu_tm4cOS_init(void);
void create_user_memory_region(int);

#endif /*__MEM_H__*/
