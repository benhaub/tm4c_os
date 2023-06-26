/**************************************************************************//**
 * @author Ben Haubrich                                                      
 * @file   scheduling_algorithm.c                                            
 * @date   March 15th, 2021                                                  
 * @details \b Synopsis: \n Algorithms that the scheduler can call to select the
 * next process
 *****************************************************************************/
#include <scheduling_algorithm.h>

//! @cond Doxygen_Suppress_Warning
/* From proc.c */
extern struct pcb ptable[MAXPROC];
//! @endcond

/**
 * Sequentially selects the next process in a fixed order every time the
 * amount of time has passed equal to the quanta. The quanta is typically
 * the tick interrupt.
 * @see start_clocktick
 * @note
 *   On the first run through all the processes, this will skip pid 0 and start
 *   at 1. After go through all the process from pid 1 to MAXPROC-1, then we
 *   will run pid 0.
 */
struct pcb* round_robin() {
/* Current index of the scheduler. */
	static unsigned int index = 0;

  index++;

  if (index >= MAXPROC) {
    index = 0;
  }

  return &ptable[index];
}
