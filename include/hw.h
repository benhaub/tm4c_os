/**************************************************************************//**
 * @author	Ben Haubrich                                                    
 * @file		hw.h                                                            
 * @date		May 16th, 2019                                                  
 * @details \b Synopsis: \n	Hardware peripheral calls for TM4C123                           
 *****************************************************************************/
#ifndef __HW_H__
#define __HW_H__

#include "types.h"

/* Systick */
void systickInit(int, int, uint32_t);
void startClocktick(int, int);
void systickDelay1ms(void);
/* LED */
void ledInit(void);
void ledRedOn(void);
void ledRedOff(void);
void ledGreenOn(void);
void ledGreenOff(void);
void ledBlueOn(void);
void ledBlueOff(void);
/* Flash Memory */
int writeFlash(void *, void *, void *);
void eraseFlash(uint32_t);
/* UART */
int uart1Init(unsigned int);
int uart1TransmitChar(char);
/* SSI */
int ssi0InitMaster(uint8_t, bool);
int ssi0Transmit(uint8_t);
uint8_t ssi0Receive();
/* GPTM */
void gptmTimerInit();
void gptmTimerStart(uint32_t);
int gptmWaitForTimeout();

/* GPIO */
int gpioWrite(int, int, int);
void gpioInit(int port);

#endif /*__HW_H__*/
