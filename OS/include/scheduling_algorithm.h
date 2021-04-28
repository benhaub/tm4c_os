/**************************************************************************//**
 * @author Ben Haubrich                                                      
 * @file   scheduling_algorithm.h                                            
 * @date    : March 15th, 2021                                                  
 * @details \b Synopsis: \n Alogorithms that the scheduler can call to select
 * the next process
 *****************************************************************************/
#include <proc.h>

struct pcb* round_robin();
