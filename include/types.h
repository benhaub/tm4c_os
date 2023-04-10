/**************************************************************************//**
 * @author	Ben Haubrich
 * @file		types.h
 * @date		June 9th, 2019
 * @details \b Synopsis:	\n Data types
 *****************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @def NULL
 * No pointer should ever be 0x0 unless you purposely trying to change the
 * vector table at runtime (don't).
 */
#define NULL (void *)0x0
/**
 * @typedef pid_t
 *   Any valid pid will always be non-zero and less than or equal to MAX_PROC.
 */
typedef uint8_t pid_t;

#endif /*__TYPES_H__*/
