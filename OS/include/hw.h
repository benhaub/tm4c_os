/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			:	hw.h																														*
 * Synopsis	:	Hardware peripheral calls for TM4C123														*
 * Date			:	May 16th, 2019																									*
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include <types.h>

/* 16 MHz PIOSC system freqency */
#define SYS_CLOCK_FREQ 4000000
/* 1ms clock tick based off the system clock frequency. */
#define CLOCK_TICK 4000
/* Supported baud rates for UART */
#define B115200 115200u

/* Systick calls */
void systick_init(void);
void start_clocktick(void);
void delay_1ms(void);
/* LED calls */
void led_init(void);
void led_ron(void);
void led_roff(void);
void led_gron(void);
void led_groff(void);
void led_blon(void);
void led_bloff(void);
/* Flash Memory calls */
int write_flash(void *, void *, void *);
void erase_flash(word);
//int protect_flash(int); Not working.
/* UART calls */
void uart1_init(unsigned int);
int uart1_tchar(char);

#endif /*__HW_H__*/
