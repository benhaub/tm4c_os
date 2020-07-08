/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : mem.c                                                           *
 * Synopsis : Memory management                                               *
 * Date     : June 13th, 2019                                                 *
 *****************************************************************************/
#include <types.h>
#include <mem.h>
#include <cstring.h>

/* Array to determine what space in ram is being used. */
/* Kernel stack usage is automatically determined at reset and stored on the */
/* stack and retrieved with KRAM_USE. */
int stackusage[SRAM_PAGES];

/* Return the index of the ram space that is not being used, or -1 if there */
/* is no space. */
int get_stackspace() {
/* Each element represents the top of the stack that will be used. 0 will */
/* use from 0 to STACK_SIZE-1, 1 will use STACK_SIZE to 2*STACK_SIZE-1, etc. */
	int i = 0;
	while(stackusage[i]) {
		i++;
		if(i > SRAM_PAGES) {
      printf("No available RAM for new stack\n\r");
			return -1;
		}
	}
	stackusage[i] = 1;
	return i;
}

inline void free_stackspace(int i) {
 	stackusage[i] = 0;
}

/* Sets all ram to be unused. */
void init_ram() {
	int i;
/* The amount of RAM usage was pushed on the stack during reset. We need to */
/* round the value up to make sure there's not stack overlap (+ 2). After a */
/* push, the processor increments the stack pointer to the next word. */
/* KRAM_USE is stored one word back (- 4). */
	for(i = 0; i < *((word *)(KRAM_USE - 4)) + 2; i++) {
		stackusage[i] = 1;
/*TODO:
 * Add assembly here to pop the stack back to the top.
 */
    __asm__("");
	}
	while(i < SRAM_PAGES) {
		stackusage[i] = 0;
		i++;
	}
}
