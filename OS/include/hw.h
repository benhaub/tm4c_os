/******************************************************************************
 * Authour	:	Ben Haubrich                                                    *
 * File			:	hw.h                                                            *
 * Synopsis	:	Hardware peripheral calls for TM4C123                           *
 * Date			:	May 16th, 2019                                                  *
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include <types.h>

/* On reset, PIOSC is the system clock */
const float SysClkFrequency;
#define MAIN_OSC_FREQ 16000000
/* Systick uses SysClk/4 so the clock tick is based of a 4MHz frequency. */
/*TODO: Need to run tests on this. I have systick set to run on SysClk. */
#define CLOCK_TICK SysClkFrequency / 4000 //1ms
/* Supported baud rates for UART */
#define B115200 115200u

/* System Control */
/*
 * @struct clocksource_config_t
 * @var oscsrc
 *   The oscillator source for the clock.
 *   0x0 - Main oscillator (MOSC)
 *   0x1 - Precision internal oscillator (PIOSC)
 *   0x2 - PIOSC/4
 *   0x3 - 30kHz Low-frequency internal osciallator
 *   0x4 - 32.768kHz external oscillator
 * @var use_pll
 *   SysClk is the output of the Phase Locked Loop with VCO of 400MHz divided
 *   by 2.
 * @var sysdiv
 *   If sysdiv is greater than 2, then the oscsrc is divided by sysdiv. If the
 *   oscsrc is sent through the PLL, then 200MHz is divided by sysdiv to produce
 *   SysClk unless div400 is set, in which case the divided value is 400.
 *   The maximum value for sysdiv is 16.
 * @var sysdiv2
 *   If sysdiv2 is greater than 4, then the oscsrc is divided by sysdiv2. If the
 *   oscsrc is sent through the PLL, then 200MHz is divided by sysdiv2 to
 *   produce SysClk unless div400 is set, in which case the divided value is 400
 *   Both sysdiv and sysdiv2 may not be used at the same time and attempts to do
 *   so will result in clock configuration failure. The maximum value for
 *   sysdiv2 is 128.
 * @var div400
 *   If set, then the PLL output is divided by 400MHz instead of 200.
 * @var sysdiv2lsb
 *   When div400 is used, sysdiv2lsb is either 1 or 0 to append an extra least
 *   significant bit to sysdiv2.
 * @note
 *   It's completely acceptable to use an oscsrc with no system clock division.
 *   If the oscsrc is the PLL with no division, then MINSYSDIV is used.
 * @see
 *   datasheet, Pg. 222 - 224
 */
struct clocksource_config_t {
  uint8_t oscsrc;
  uint8_t use_pll;
  uint8_t sysdiv;
  uint8_t sysdiv2;
  uint8_t div400;
  uint8_t sysdiv2lsb;
};

int set_clocksource(struct clocksource_config_t, float *); 
/* Systick */
void systick_init(void);
void start_clocktick(void);
void delay_1ms(void);
/* LED */
void led_init(void);
void led_ron(void);
void led_roff(void);
void led_gron(void);
void led_groff(void);
void led_blon(void);
void led_bloff(void);
/* Flash Memory */
int write_flash(void *, void *, void *);
void erase_flash(uint32_t);
/* UART */
int uart1_init(unsigned int);
int uart1_tchar(char);
/* SSI */
int ssi0_init_master(int, int, int, int);
void ssi0_transmit(int);

#endif /*__HW_H__*/
