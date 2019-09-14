/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			:	hw.h																														*
 * Synopsis	:	Hardware peripheral calls for TM4C123														*
 * Date			:	May 16th, 2019																									*
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include <types.h>

/* 16 MHz PIOSC freqency */
#define CLOCK_FREQ 16000000
/* 1ms clock tick */
#define CLOCK_TICK 16000
/* BOOTCFG key for enabling flash writes */
#define BOOTKEY 0x71D5

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
/* Flash Memory calls */
int write_flash(void *, void *);
void protect_flash(word);

#endif /*__HW_H__*/
