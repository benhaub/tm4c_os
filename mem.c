/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    mem.c                                                           
 * @date    June 13th, 2019                                                 
 * @details \b Synopsis: \n  Memory management                                               
 *****************************************************************************/
#include <mem.h>
#include <kernel_services.h>
#include <tm4c123gh6pm.h>

/**
 * @var stackusage
 *   Array to determine what space in ram is being used.
 *   Kernel stack usage is automatically determined at reset and stored on the
 *   stack and retrieved with KRAM_USE.
*/
int stackusage[SRAM_PAGES];

/**
 * @var stacksize_pow2
 *   Used for the MPU. Holds the exponent of the stack size with base 2.
 *   For example, if the stacksize is 1024, then this will hold 10.
 * @sa mpu_tm4cOS_init
 * @sa create_user_memory_region
 */
int stacksize_pow2;

/**
 * @brief
 *   Find a section of memory for a process to use as its stack.
 * Each element represents the bottom of the stack that will be used. 0 will
 * use from 0 to STACK_SIZE-4, 1 will use STACK_SIZE-4 to 2*STACK_SIZE-4, etc.
 * @return
 *   Return the index of the ram page that is not being used, or -1 if there
 *   is no page.
 * @sa reserveproc, stacktop, stackbottom
 */
int get_stackpage() {
	int i = 0;
	while(stackusage[i]) {
		i++;
		if(i > SRAM_PAGES) {
      syswrite("No available RAM for new stack\n\r");
			return -1;
		}
	}
	stackusage[i] = 1;
	return i;
}

/**
 * @brief
 *   Mark the the space for this processes stack as unused.
 */
inline void free_stackpage(int i) {
 	stackusage[i] = 0;
}

/**
 * @brief
 *   Sets all ram to be unused.
 */
void init_ram() {
	int i;
/* The amount of RAM usage was pushed on the stack during reset. We need to */
/* round the value up to make sure there's not stack overlap (+ 1). After a */
/* push, the processor increments the stack pointer to the next uint32_t. */
/* KRAM_USE is stored one uint32_t back (- 1). If you edit this calculation, also*/
/* make the same changes to user_init() in proc.c. */
	for(i = 0; i < *(KRAM_USE - 1) + 1; i++) {
		stackusage[i] = 1;
	}
	while(i < SRAM_PAGES) {
		stackusage[i] = 0;
		i++;
	}
}

/**
 * @brief
 *   Initialize the MPU
 * Initialize the MPU specifically for tm4cOS. During OS initialization, any
 * memory access from non-privledged software will cause a memory fault.
 */
void mpuInit() {
  /* Find what the stack size is as a power of two. */
  int i;
  for(i = 0; i < 31; i++) {
    if(1 == STACK_SIZE >> i ) {
      break;
    }
  }
  if (STACK_SIZE > 1 << i) {
    i++;
  }

  stacksize_pow2 = i;
  NVIC_MPU_NUMBER_R &= ~0x7;
  NVIC_MPU_NUMBER_R |= 0x5;
  /* The region occupies the entire memory map */
  NVIC_MPU_ATTR_R |= (0x1F << 1);
  /* Privledged is RW, unprivledged is RO. Pg.129, datasheet. */
  NVIC_MPU_ATTR_R |= (2 << 24);
  /* Enable this region. Pg. 128, datasheet. */
  NVIC_MPU_ATTR_R |= 0x1;

  //Now move on to the next region to allow RW access to global variables.
  /* Find what the _data section size is as a power of two. */
  int dataSectionSizePow2;
  for(i = 0; i < 31; i++) {
    if(1 == (&_ebss - &_bss) >> i ) {
      break;
    }
  }
  if ((&_ebss - &_bss) > 1 << i) {
    i++;
  }
  
  dataSectionSizePow2 = i;

  NVIC_MPU_NUMBER_R &= ~0x7;
  NVIC_MPU_NUMBER_R |= 0x6;
  NVIC_MPU_BASE_R &= ~0xFFFFFFE0;
  NVIC_MPU_BASE_R |= (uint32_t)&_bss;
  NVIC_MPU_BASE_R |= 1 << 4; //Set the valid bit to update MPU number and base
  NVIC_MPU_ATTR_R &= ~(0x1F << 1);
  NVIC_MPU_ATTR_R |= ((dataSectionSizePow2-1) << 1); //Pg.192, datasheet.
  NVIC_MPU_ATTR_R &= ~(0x3 << 24);
  NVIC_MPU_ATTR_R |= (0x3 << 24); //Full access to this memory region.
  NVIC_MPU_ATTR_R |= 0x1; //Enable this memory region.
  /* Enable the MPU. */
  NVIC_MPU_CTRL_R |= 0x1;
}

/**
 * @brief
 *   Alter the MPU memory region so that the process with ram page rampg may
 *   only access its own stack and global data.
 *   
 * Create a region of memory for a user process to operate in. The process may
 * write only to it's stack. Reads are allowed anywhere in flash or RAM except
 * for the memory region of RAM association with the kernel.
 * The rest of the memory space is writable by privledged software
 * (the kernel) only. The region in number 7 which overlaps the region in
 * number 6 will have all of it's attributes applied to that region since
 * it's number is higher.
 *
 * @param rampg
 *   The rampg that the process is using.
 * @pre
 *   The MPU must be enabled for this function to have any effect.
 * @sa get_stackspace
 */
void create_user_memory_region(int rampg) {
  if(0 == (NVIC_MPU_CTRL_R & 0x1)) {
    return;
  }
  NVIC_MPU_NUMBER_R &= ~0x7;
  NVIC_MPU_NUMBER_R |= 0x7;
  NVIC_MPU_BASE_R &= ~0xFFFFFFE0;
  NVIC_MPU_BASE_R |= stacktop(rampg) - STACK_SIZE + 4;
  NVIC_MPU_ATTR_R &= ~(0x1F << 1);
  NVIC_MPU_ATTR_R |= ((stacksize_pow2-1) << 1); //Pg.192, datasheet.
  NVIC_MPU_ATTR_R &= ~(0x3 << 24);
  NVIC_MPU_ATTR_R |= (0x3 << 24); //Full access to this memory region.
  NVIC_MPU_ATTR_R |= 0x1; //Enable this memory region.
}
