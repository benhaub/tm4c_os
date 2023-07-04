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
int ssi0InitMaster();
int ssi0_transmit(uint8_t);
uint8_t ssi0_receive();
/* GPTM */
void gptmTimerInit();
void gptmTimerStart(uint32_t);
int gptmWaitForTimeout();

/* GPIO */
int gpioWrite(int, int, int);
void gpioInit(int port);

#endif /*__HW_H__*/
