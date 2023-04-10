/**************************************************************************//**
 * @author	Ben Haubrich                                                    
 * @file		hw.h                                                            
 * @date		May 16th, 2019                                                  
 * @details \b Synopsis: \n	Hardware peripheral calls for TM4C123                           
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include <types.h>

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
int ssi0_init_master();
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
 * @var timer_config_t::instance
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
void gptm_start_timer(gptm_timer_t, uint8_t);
uint8_t gptm_timeout(gptm_timer_t, uint8_t);
void gptm_delay_1ms(gptm_timer_t);

void gpio_write(int, int, int);

#endif /*__HW_H__*/
