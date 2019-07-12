/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	mem.c
 * Synopsis	:	Memory management
 * Date			:	June 13th, 2019
 *****************************************************************************/
/* From context.s */
extern int KRAM_USE;
/* Bit array to determine what space in ram is being used. */
/* 0 is not being used while 1 is. The first 1KB is used by the kernel. */
int stackusage[32];

/* Return the index of the ram space that is not being used. */
int get_stackspace() {
/* Each element represents the top of the stack that will be used. 0 will */
/* use stack space from 0x0 to 0x1000, 1 will use 0x1000 to 0x2000 etc. */
/* KRAM_USE tells use where to start, since the numbers before that are RAM */
/* space that use being used by the kernel. */
	int i = KRAM_USE;
	while(stackusage[i]) {
		i++;
	}
	stackusage[i] = 1;
	return i;
}

/* Sets all ram to be unused. */
void init_ram() {
	int i;
	for(i = 0; i < 32; i++) {
		stackusage[i] = 0;
	}
}
