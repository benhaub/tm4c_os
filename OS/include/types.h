/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	types.h
 * Synopsis	:	Data types
 * Date			:	June 9th, 2019
 *****************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

/* Half of the largest integer possible. */
#define HALF_INT (1 << 31)
/* Largest integer possible. */
#define MAX_INT ((1 << 32) - 1)
/* Define the size of a word (32-bits) */
typedef unsigned long int word;
/* No pointer should ever be 0x0 */
#define NULL (void *)0x0

#endif /*__TYPES_H__*/
