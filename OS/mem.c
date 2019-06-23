/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	mem.c
 * Synopsis	:	Memory management
 * Date			:	June 13th, 2019
 *****************************************************************************/

/* Bit array to determine what space in ram is being used. */
/* 0 is not being used while 1 is. The first 1KB is used by the kernel. */
int stackusage[32];

/* Return the index of the ram space that is not being used. */
int get_stackspace() {
	/* position zero is used by the kernel. */
	int i = 1;
	while(stackusage[i]) {
		i++;
	}
	stackusage[i] = 0;
	return i;
}

/* Sets all ram to be unused. */
void init_ram() {
	int i;
	for(i = 0; i < 32; i++) {
		stackusage[i] = 0;
	}
}
