/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : mem.c                                                           *
 * Synopsis : Memory management                                               *
 * Date     : June 13th, 2019                                                 *
 *****************************************************************************/
#include <mem.h>
#include <cstring.h>
#include <tm4c123gh6pm.h>

/* Array to determine what space in ram is being used. */
/* Kernel stack usage is automatically determined at reset and stored on the */
/* stack and retrieved with KRAM_USE. */
int stackusage[SRAM_PAGES];
/* Used for the MPU. Holds the exponent of the stack size with base 2. */
int stacksize_pow2;

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

/*
 * Sets all ram to be unused.
 */
void init_ram() {
	int i;
/* The amount of RAM usage was pushed on the stack during reset. We need to */
/* round the value up to make sure there's not stack overlap (+ 2). After a */
/* push, the processor increments the stack pointer to the next word. */
/* KRAM_USE is stored one word back (- 4). */
	for(i = 0; i < *((word *)(KRAM_USE - 4)) + 2; i++) {
		stackusage[i] = 1;
	}
	while(i < SRAM_PAGES) {
		stackusage[i] = 0;
		i++;
	}
}

/*
 * Initialize the MPU specifically for tm4cOS. During OS initialization, any
 * memory access from non-privledged software will cause a memory fault.
 */
void mpu_tm4cOS_init() {
  /* Find what the stack size is as a power of two. */
  int i;
  for(i = 0; i < 31; i++) {
    if(1 == STACK_SIZE >> i ) {
      break;
    }
  }
  stacksize_pow2 = i;
  NVIC_MPU_NUMBER_R |= 0x5;
  /* The region occupies the entire memory map */
  NVIC_MPU_ATTR_R |= (0x1F << 1);
  /* Privledged is RW, unprivledged is RO. Pg.129, datasheet. */
  NVIC_MPU_ATTR_R |= (2 << 24);
  /* Enable this region. */
  NVIC_MPU_ATTR_R |= 0x1;
  /* Enable the MPU. */
  NVIC_MPU_CTRL_R |= 0x1;
}

/*
 * Create a region of memory for a user process to operate in. The process may
 * write only to it's stack. Reads are allowed anywhere in flash or RAM except
 * for the memory region of RAM association with the kernel.
 * The rest of the memory space is writable by privledged software
 * (the kernel) only. The region in number 6 which overlaps the region in
 * number 5 will have all of it's attributes applied to that region since
 * it's number is higher.
 * @param tos
 *   The top of stack associated with the user process
 */
void create_user_memory_region(int rampg) {
  NVIC_MPU_NUMBER_R |= 0x6;
  NVIC_MPU_BASE_R |= stacktop(rampg) - STACK_SIZE + 4;
  NVIC_MPU_ATTR_R |= ((stacksize_pow2-1) << 1); //Pg.192, datasheet.
  NVIC_MPU_ATTR_R |= (0x3 << 24); //Full access to this memory region.
  NVIC_MPU_ATTR_R |= 0x1; //Enable this memory region.
}
