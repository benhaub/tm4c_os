/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : mem.c                                                           *
 * Synopsis : Memory management                                               *
 * Date     : June 13th, 2019                                                 *
 *****************************************************************************/
#include <types.h>
#include <mem.h>

/* From vectors.s */
extern int KRAM_USE;
/* Array to determine what space in ram is being used. */
/* Kernel stack usage is automatically determined at reset and stored on the */
/* stack and retrieved with KRAM_USE. */
int stackusage[SRAM_PAGES];

/* Return the index of the ram space that is not being used, or -1 if there */
/* is no space. */
int get_stackspace() {
/* Each element represents the top of the stack that will be used. 0 will */
/* use stack space from 0x0 to 0x1000, 1 will use 0x1000 to 0x2000 etc. */
/* KRAM_USE tells use where to start, since the numbers before that are RAM */
/* space that use being used by the kernel. */
	int i = 0;
	while(stackusage[i]) {
		i++;
		if(i > SRAM_PAGES) {
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
/* round the value up to make sure there's not stack overlap (+ 1). After a */
/* push, the processor increments the stack pointer to the next word. */
/* KRAM_USE is stored one word back (- 4). */
	for(i = 0; i < *((word *)(KRAM_USE - 4)) + 1; i++) {
		stackusage[i] = 1;
	}
	while(i < SRAM_PAGES) {
		stackusage[i] = 0;
		i++;
	}
}
