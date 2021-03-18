/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	types.h
 * Synopsis	:	Data types
 * Date			:	June 9th, 2019
 *****************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

/* No pointer should ever be 0x0 unless you purposely trying to change the */
/* vector table at runtime (don't). */
#define NULL (void *)0x0
/**
 * @typedef pid_t
 *   Any valid pid will always be non-negative.
 *TODO:
 * I don't like this solution. Would rather have the only invalid pid be zero.
 * and make pid_t and unsigned 8 bit int.
 */
typedef int8_t pid_t;

#endif /*__TYPES_H__*/
