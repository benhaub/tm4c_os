/**************************************************************************//**
 * @author Ben Haubrich                                                      
 * @file   scheduling_algorithm.c                                            
 * @date   March 15th, 2021                                                  
 * @details \b Synopsis: \n Algorithms that the scheduler can call to select the
 * next process
 *****************************************************************************/
#include <scheduling_algorithm.h>

/* From proc.c */
extern struct pcb ptable[MAX_PROC];
/**
 * Sequentially selects the next process in a fixed order every time the
 * amount of time has passed equal to the quanta. The quanta is typically
 * the tick interrupt.
 * @see start_clocktick
 */
struct pcb* round_robin() {
/* Current index of the scheduler. */
	static unsigned int index = 0;
  if(index >= MAX_PROC) {
    index = 0;
  }
  else {
    index++;
  }
  return &ptable[index];
}
