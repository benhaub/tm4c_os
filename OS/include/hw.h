/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			:	hw.h																														*
 * Synopsis	:	Hardware peripheral calls for TM4C123														*
 * Date			:	May 16th, 2019																									*
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

/* 16/4 MHz crystal freqency */
#define CLOCK_FREQ 4000000
/* 1ms clock tick */
#define CLOCK_TICK 4000

/* Systick calls */
void delay_1ms(void);
void start_clocktick(void);
/* LED calls */
void led_init(void);
void led_ron(void);
void led_roff(void);
void led_gron(void);
void led_groff(void);
void led_blon(void);
void led_bloff(void);

#endif /*__HW_H__*/
