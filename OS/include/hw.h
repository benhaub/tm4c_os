/**************************************************************************//**
 * @author	Ben Haubrich                                                    
 * @file		hw.h                                                            
 * @date		May 16th, 2019                                                  
 * @details \b Synopsis: \n	Hardware peripheral calls for TM4C123                           
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include <types.h>

///@var SysClkFrequency
///On reset, PIOSC is the system clock
const float SysClkFrequency;
///@def MAIN_OSC_FREQ
///External 16.0MHz crystal (Y2) Frequency
#define MAIN_OSC_FREQ 16000000
///@def PIOSC_FREQ
///Precision Internal Oscillator Frequency
#define PIOSC_FREQ 16000000

/* System Control */
/**
 * @struct clocksource_config_t
 * @brief
 *   clocksource config type used to initialize the clocks.
 * @note
 *   It's completely acceptable to use an oscsrc with no system clock division.
 *   If the oscsrc is the PLL with no division, then MINSYSDIV is used.
 * @see
 *   datasheet, Pg. 222 - 224
 * @var clocksource_config_t::oscsrc
 *   The oscillator source for the clock.
 *   \b 0x0 - Main oscillator (MOSC) \n
 *   \b 0x1 - Precision internal oscillator (PIOSC) \n
 *   \b 0x2 - PIOSC/4 \n
 *   \b 0x3 - 30kHz Low-frequency internal osciallator \n
 *   \b 0x4 - 32.768kHz external oscillator
 * @var clocksource_config_t::use_pll
 *   SysClk is the output of the Phase Locked Loop with VCO of 400MHz divided
 *   by 2.
 * @var clocksource_config_t::sysdiv
 *   If sysdiv is greater than 2, then the oscsrc is divided by sysdiv. If the
 *   oscsrc is sent through the PLL, then 200MHz is divided by sysdiv to produce
 *   SysClk unless div400 is set, in which case the divided value is 400.
 *   The maximum value for sysdiv is 16.
 * @var clocksource_config_t::sysdiv2
 *   If sysdiv2 is greater than 2, then the oscsrc is divided by sysdiv2. If the
 *   oscsrc is sent through the PLL, then 200MHz is divided by sysdiv2 to
 *   produce SysClk unless div400 is set, in which case the divided value is 400
 *   Both sysdiv and sysdiv2 may not be used at the same time and attempts to do
 *   so will result in clock configuration failure. The maximum value for
 *   sysdiv2 is 128.
 * @var clocksource_config_t::div400
 *   If set, then the PLL output is divided by 400MHz instead of 200. When
 *   div400 is used, sysdiv2 must be greater than 4.
 * @var clocksource_config_t::sysdiv2lsb
 *   When div400 is used, sysdiv2lsb is either 1 or 0 to append an extra least
 *   significant bit to sysdiv2.
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
void systick_init(int, int, uint32_t);
void start_clocktick(int, int);
void systick_delay_1ms(void);
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
int ssi0_transmit(uint8_t);
uint8_t ssi0_receive();
/* GPTM */
/**
 * @struct timer_config_t
 * @brief
 *   Initialize a general purpose timer
 * @note
 * @see
 *   datasheet, Pg. 722
 * @var timer_config_t::periodic
 *   If set, then counter will reset when time runs out, else it will act
 *   as a one-shot timer and will not reset when the time runs out.
 * @var timer_config_t::capture
 *   set for capture mode
 * @var timer_config_t::pwm
 *   set for pwm mode
 * @var timer_config_t::direction
 *   This bit is ignored when rtc or pwm is set because they are fixed to count
 *   up or down respectively.
 *   0 - count down
 *   1 - count up
 * @var timer_config_t::edge_mode
 *   0 - edge-count mode
 *   1 - edge-time mode
 * @var timer_config_t::wait_on_trigger
 *   0 - start the timer as soon as it's enabled
 *   1 - do not start the timer until it receive a trigger from the previous
 *   timer in the daisy chain
 * @var timer_config_t::snapshot_mode
 *   0 - disable snap-shot mode
 *   1 - enable snap-shot mode 
 * @var timer_config_t::interval
 *   Value that the timer will count up/down to.
 * @var timer_config_t instance
 *   The timer init function will fill this value when it returns successfully
 *   0 - Timer A was initialized
 *   1 - Timer B was initialized
 */
struct timer_config_t {
  uint8_t periodic;
  uint8_t capture;
  uint8_t pwm;
  uint8_t direction;
  uint8_t edge_mode;
  uint8_t wait_on_trigger;
  uint8_t snapshot_mode;
  uint32_t interval;
  uint8_t instance;
};
typedef volatile unsigned long * gptm_timer_t;

gptm_timer_t gptm_timer_init(struct timer_config_t *);
uint8_t timer0A_init(struct timer_config_t *);
uint8_t timer0B_init(struct timer_config_t *);
uint8_t timer1A_init(struct timer_config_t *);
uint8_t timer1B_init(struct timer_config_t *);
uint8_t timer2A_init(struct timer_config_t *);
uint8_t timer2B_init(struct timer_config_t *);
uint8_t timer3A_init(struct timer_config_t *);
uint8_t timer3B_init(struct timer_config_t *);
uint8_t timer4A_init(struct timer_config_t *);
uint8_t timer4B_init(struct timer_config_t *);
uint8_t timer5A_init(struct timer_config_t *);
uint8_t timer5B_init(struct timer_config_t *);
void gptm_delay_1ms();

#endif /*__HW_H__*/
