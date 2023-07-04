/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    hw.c                                                            
 * @date    May 16th, 2019                                                
 * @details \b Synopsis: \n Hardware peripheral calls for TM4C123                         
 *****************************************************************************/
//tm4c includes
#include "hw.h"
//Texas Instruments includes
#include "tm4c123gh6pm.h"
#include "ssi.h"
#include "gpio.h"
#include "timer.h"
#include "sysctl.h"
#include "hw_memmap.h"

//TEMP
#include "kernel_services.h"


/*********************************SYSTICK*************************************/

/* PIOSC clock is default. See Pg. 219 and Pg. 256-257. RCC is left at */
/* default, so what the datasheet refers to as the "System Clock" is the same */
/* as the OSCSRC since the BYPASS is not enabled, nor is SYSDIV. */

/**
 * @brief systick_init
 *   Initialize the SysTick counter.
 * @param clksrc \n
 *   \b 0x0 - PIOSC/4 \n
 *   \b 0x1 - SysClk
 * @param inten \n
 *   \b 0x0 - No interrupts when NVIC_ST_CURRENT reaches 0 \n
 *   \b 0x1 - Interrupt when NVIC_ST_CURRENT reach 0
 * @param reload
 *   The value that systick should count to before resetting and counting again.
 * @post
 *   The counter will begin counting.
 */
void systick_init(int clksrc, int inten, uint32_t reload) {
    /* Make sure systick is disabled for initialization */
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = reload;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R |= (((clksrc<<1) | inten) << 1);
    NVIC_ST_CTRL_R |= 0x1; //Re-enable SysTick
    return;
}

/**
 * Start a system clock tick with interrupts enabled. Interrupts frequencies
 * must be such that the OS has time to complete scheduling and context
 * switching.
 * @param clksrc \n
 *   \b 0x0 - PIOSC/4 \n
 *   \b 0x1 - SysClk
 * @param period
 *   The number of milliseconds before an interrupt is activated
 */
void start_clocktick(int clksrc, int period) {
  NVIC_ST_CTRL_R = 0;
  if(clksrc) {
    NVIC_ST_RELOAD_R = (SysCtlClockGet() / 1000) * period;
  }
  else {
    NVIC_ST_RELOAD_R = (SysCtlClockGet() / 4000) * period;
  }
  NVIC_ST_CURRENT_R = 0;
  if(clksrc) {
    NVIC_ST_CTRL_R = 0x7;
  }
  else {
    NVIC_ST_CTRL_R  = 0x3;
  }
  return;
}

/**
 * 1ms delay
 */
void systick_delay_1ms() {
    NVIC_ST_RELOAD_R = (SysCtlClockGet() / 1000);
    NVIC_ST_CURRENT_R = 0;
    while(!(NVIC_ST_CTRL_R & (1 << 16)));
    return;
}

/************************************LED**************************************/

/* You can find which pins are LEDs by seeing the Tiva C Series LaunchPad */
/* Evaluation Kit User's Manual, Pg.9. The Initializaton process is found */
/* in the datasheet on Pg.656. */

/**
 * @brief
 *   Initialize PortF for LED operation. This function must be run before the
 *   LEDs can be used.
 */
void led_init() {
    SYSCTL_RCGCGPIO_R |= (1 << 5); //Enable port F
    /* Dummy instruction to let the clock settle */
    #pragma GCC diagnostic push //Remember the diagnostic state
    #pragma GCC diagnostic ignored "-Wunused-variable" //choose to ignore
    unsigned int dlyclk = SYSCTL_RCGCGPIO_R;
    #pragma GCC diagnostic push //push the state back to before we ingnored
    GPIO_PORTF_DIR_R |= (1 << 1); //Direction is output
    GPIO_PORTF_DIR_R |= (1 << 2);
    GPIO_PORTF_DIR_R |= (1 << 3);
    GPIO_PORTF_AFSEL_R &= ~(1 << 0); //No alt function. Function as GPIO
    GPIO_PORTF_DEN_R |= (1 << 1); //Digital Enable
    GPIO_PORTF_DEN_R |= (1 << 2);
    GPIO_PORTF_DEN_R |= (1 << 3);
    return;
}

/**
 * @brief
 *   Turn the red led on
 */
void led_ron() {
    GPIO_PORTF_DATA_R |= (1 << 1);
    return;
}

/**
 * @brief
 *   Turn off red led
 */
void led_roff() {
    GPIO_PORTF_DATA_R &= ~(1 << 1);
    return;
}

/**
 * @brief
 *   Turn the green led on
 */
void led_gron() {
    GPIO_PORTF_DATA_R |= (1 << 3);
    return;
}

/**
 * @brief
 *   Turn off the green led on
 */
void led_groff() {
    GPIO_PORTF_DATA_R &= ~(1 << 3);
    return;
}

/**
 * @brief
 *   Turn the blue led on
 */
void led_blon() {
    GPIO_PORTF_DATA_R |= (1 << 2);
    return;
}

/**
 * @brief
 *   Turn the blue led off
 */
void led_bloff() {
    GPIO_PORTF_DATA_R &= ~(1 << 2);
    return;
}

/******************************Flash Memory***********************************/

/**
 * @brief
 *   Write to flash memory
 *
 * Follows the procedure on Pg. 532, datasheet.
 * Write values in ram from starting from saddr and ending at eaddr into
 * flash memory that starts at faddr. This function allows you to write a
 * maximum of 1KB per call. For successive block writes, make multiple calls.
 * This call requires more than 1KB of stack space.
 *
 * @param saddr
 *   Starting address of the data to be written
 * @param eaddr
 *   Ending address of the data to be written
 * @param faddr
 *   The flash address where the write should start at
 * @return
 *   0 on success, -1 on error.
 */
int write_flash(void *saddr, void *eaddr, void *faddr) {
  if((uint32_t)faddr <= 0x1000) {
    while(1);
  }
  if((uint32_t)eaddr - (uint32_t)saddr > 1*1024) {
    return -1;
  }
/* Align the flash address to the nearest 1KB boundary */
  FLASH_FMA_R = ((uint32_t)faddr & ~(0x3FF));
/* If the write can't be done with one set of buffer registers, then set the */
/* cflag to re-fill the buffer regs and continue writing. */
  int cflag = 0;
/* current address being written to flash copy */
  uint32_t *curraddr = (uint32_t *)saddr;
/* The fpage_os is the offset of FLASH_FMA_R where the flash writes begin at.*/
/* This address must be uint32_t aligned. */
  uint32_t fpage_os = (uint32_t)faddr & 0xFC;
/* Since 0's can not be programmed back to a 1. We have to copy and erase */
/* flash before making the write. */
  uint32_t fcopy[256]; /* 1KB of space */
  int i, j;
  for(i = 0; i < 256; i++) {
      fcopy[i] = *((uint32_t *)FLASH_FMA_R + i);
  }
/* Write the new data into the copy we've obtained. */
  if(fpage_os != 0) {
    i = (fpage_os >> 2);
  }
  else {
    i = 0;
  }
/* Write in the new values into the copy we've obtained. */
  while(curraddr < (uint32_t *)eaddr) {
    fcopy[i] = *curraddr;
    curraddr += 1;
    i++;
  }
/* Erase the 1KB block of flash starting at faddr. */
  FLASH_FMC_R |= FLASH_FMC_WRKEY | FLASH_FMC_ERASE;
  while(FLASH_FMC_R);
  j = 0;
/* Copy the modified fcopy back into the flash write buffers. */
Write:
  i = 0;
  while((FLASH_FMA_R + i*4) < (FLASH_FMA_R + 1*1024)) {
      *(&FLASH_FWBN_R + i) = fcopy[j];
    i++;
    j++;
/* Check to see if all the flash buffers have filled. If they have, then we */
/* need to write and continue filling the buffers after the write has */
/* completed. */
    if(0xFFFFFFFF == FLASH_FWBVAL_R) {
        cflag = 1;
        break;
    }
  }
/* Initiate the write sequence */
  FLASH_FMC2_R |= FLASH_FMC_WRKEY | FLASH_FMC2_WRBUF;
/* Wait for WRBUF to clear */
  while(FLASH_FMC2_R);
/* Check raw interrupt status for any errors, then clear it. */
  if(FLASH_FCRIS_R & FLASH_FCRIS_PROGRIS) {
    FLASH_FCMISC_R |= FLASH_FCMISC_PROGMISC;
    return -1;
  }
  else if(FLASH_FCRIS_R & FLASH_FCRIS_ERRIS) {
    FLASH_FCMISC_R |= FLASH_FCMISC_ERMISC;
    return -1;
  }
  else if(FLASH_FCRIS_R & FLASH_FCRIS_INVDRIS) {
    FLASH_FCMISC_R |= FLASH_FCMISC_INVDMISC;
    return -1;
  }
  else if(FLASH_FCRIS_R & FLASH_FCRIS_VOLTRIS) {
    FLASH_FCMISC_R |= FLASH_FCMISC_VOLTMISC;
    return -1;
  }
/* Load the buffer registers again if we need to from where we left off*/
  if(1 == cflag) {
      cflag = 0;
    FLASH_FMA_R = FLASH_FMA_R + i*4;
    goto Write;
  }
  return 0;
}

/**
 * @brief
 *   Erase the 1KB flash page that contains the address pageaddr.
 */
void erase_flash(uint32_t pageaddr) {
/* Align the flash address to the nearest 1KB boundary */
    FLASH_FMA_R = pageaddr & ~0x3FF;
/* Erase the 1KB block of flash starting at pageaddr. */
  FLASH_FMC_R |= FLASH_FMC_WRKEY | FLASH_FMC_ERASE;
  while(FLASH_FMC_R);
  return;
}

/***********************************UART**************************************/

/**
 * Initialize uart module 1 to 8N1. Follows the initialization procedure on
 * Pg. 902 of the data sheet. PB0 and PB1 are used for RX and TX respectively.
 * 2mA and default slew are rate are used. Only PB1 TX is enabled since this.
 * uart is used for the kernel and user to print output.
 * @param baud
 *   The baud rate to be used for the module
 * @return
 *   0 on success, -1 on failure.
 */
int uart1_init(unsigned int baud) {
/* baud rate divisor. */
  float brd;
/* integer and float parts of the baud rate divisor. */
  int ibrd, fbrd;
/* Enable run mode for uart module 1. */
  SYSCTL_RCGCUART_R |= (1 << 1);
/* Enable run mode for GPIO Port B module (GPIOPB). */
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  if(0 != (GPIO_PORTB_AFSEL_R & (1 << 1))) {
    return -1;
  }
  GPIO_PORTB_AFSEL_R |= (1 << 1); //UART1 alt function for PB1.
  GPIO_PORTB_PCTL_R |= (1 << 4); //Transmit function for PB1.
/* Initialize GPIOPB1. Pg. 656 initialization procedure. */
  GPIO_PORTB_DEN_R |= (1 << 1); //Digital input/output, as opposed to analog.
  GPIO_PORTB_ODR_R &= ~(1 << 1); //Open drain.
/* Continue on with UART init by first disabling it during setup. */
  UART1_CTL_R &= ~0x1;
/* Set the End Of Transmission bit */
  UART1_CTL_R |= (1 << 4);
/* Use the PIOSC as UARTSysClk and generate baud rates. 16 is the divisor */
/* of the system clock for the UART clock obtained from the value of the HSE */
/* bit in the UART control register. */
  UART1_CC_R &= ~0xF;
  UART1_CC_R |= 0x5;
  if(UART1_CTL_R & (1 << 5)) {
    brd = (float)16E6 / (8 * baud);
  }
  else {
    brd = (float)16E6 / (16 * baud);
  }
  ibrd = brd;
  fbrd = (brd - ibrd) * 64 + 0.5;
  UART1_IBRD_R = ibrd;
  UART1_FBRD_R = fbrd;
  UART1_LCRH_R |= (3 << 6); //8 bits per frame. Also updates BRD regs.
  UART1_LCRH_R |= (1 << 4); //FIFO enabled.
/* This UART does not receive since it's meant for kernel and user output. */
  UART1_CTL_R &= ~(1 << 9);
/* Enable the UART for use. */
  UART1_CTL_R |= 0x1;
  return 0;
}

/**
 * Write a character to the FIFO and initiate a transfer.
 * @param data
 *   8 bit data to transmit.
 * @return
 *   0 if the data was written to the FIFO, -1 otherwise. The return
 *   value does NOT indicate a sucessful transmission, only that data was
 *   transferred.
 */
int uart1_tchar(char data) {
/* Make sure the FIFO is not full. */
  if(UART1_FR_R & (1 << 5)) {
    return -1;
  }
  UART1_DR_R = data;
/* Wait until the uart is no longer busy transmitting data. */
  while(UART1_FR_R & (1 << 3));
/* If the UART is not busy anymore, but the FIFO is still full then it's */
/* failed to transmit the data. */
  if((UART1_FR_R & (1 << 7)) == 0) {
    return -1;
  }
/* Make sure all the data has left the uart's serializer */
  if(!(UART1_RIS_R & (1 << 5))) {
    return -1;
  }
  return 0;
}

/*************************************SSI*************************************/

/**
 * @brief
 *   Initialize SSI0
 * @return
 *   non-zero on failure, 0 otherwise
 *
 */
int ssi0InitMaster() {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0));
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

/* There is no need to explicitly set the alternate function bits since */
/* Port A's default function is set to the SSI0 signals. Instead we'll */
/* check to make sure they haven't been changed. */
  if((0x2 << 8) != (GPIO_PORTA_PCTL_R & 0xF << 8)) {
    return -1;
  }
  else if((0x2 << 12) != (GPIO_PORTA_PCTL_R & 0xF << 12)) {
    return -1;
  }
  else if((0x2 << 16) != (GPIO_PORTA_PCTL_R & 0xF << 16)) {
    return -1;
  }
  else if((0x2 << 20) != (GPIO_PORTA_PCTL_R & 0xF << 20)) {
    return -1;
  }

  SSIDisable(SSI0_BASE);

  SSIConfigSetExpClk(SSI0_BASE,
      SysCtlClockGet(),
      SSI_FRF_MOTO_MODE_0,
      SSI_MODE_MASTER,
      SysCtlClockGet() / 5,
      8);

  GPIO_PORTA_AFSEL_R |= 0x3C;
  GPIO_PORTA_DEN_R |= 0x3C;

  NVIC_EN0_R |= 1 << 7;
  SSIIntEnable(SSI0_BASE, SSI_RXFF | SSI_RXTO | SSI_RXOR);
  SSIEnable(SSI0_BASE);
  return 0;
}

/**
 * @brief
 *   Blocking SSI data transmission SSI0.
 * @param data
 *   The data to transmit
 * @return
 *   non-zero on failure, 0 otherwise
 */
int ssi0_transmit(uint8_t data) {
  SSIDataPut(0x40008000, data);
  return 0;
}

/**
 * @brief
 *   Receive SSI data
 * @return
 *   The SSI data
 */
uint8_t ssi0_receive() {
  uint32_t data;
  //Send dummy data to drive the clock in order to receive.
  SSIDataPut(SSI0_BASE, 0);
  SSIDataGet(SSI0_BASE, &data);
  return (data & 0xFF);
}

void ssi0_handler() {
    uint32_t status = SSIIntStatus(SSI0_BASE, true);
    SSIIntClear(SSI0_BASE, SSI_TXEOT | SSI_DMATX | SSI_DMARX | SSI_TXFF| SSI_RXFF | SSI_RXTO | SSI_RXOR);
    if (status & SSI_RXOR)
       return;
    else if ((status & SSI_RXFF) || (status & SSI_RXTO)) {
      uint8_t data = (SSI0_DR_R << 1);
    }
    return;
}
/*****************************General Purpose Timer****************************/
void gptmTimerInit() {
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
  TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
}

void gptmTimerStart(uint32_t timeoutValueMs) {
  TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/1000)*timeoutValueMs);
  TimerEnable(TIMER0_BASE, TIMER_A);
}

/**
 * @brief
 *   Returns 0 when the timer has expired.
 */
int gptmWaitForTimeout() {
  if (TIMER_TIMA_TIMEOUT & TimerIntStatus(TIMER0_BASE, false)) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    return 0;
  }
  return 1;
}

/*************************************GPIO************************************/
void gpioInit(int port) {
  switch (port) {
    case 0:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
      }
      break;
    case 1:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
      }
      break;
    case 2:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
      }
      break;
    case 3:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
      }
      break;
    case 4:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
      }
      break;
    case 5:
      if (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
      }
      break;
  }
}

/**
 * @pre
 *   call gpioInit() first.
 */
int gpioWrite(int port, int pin, int state) {
  uint8_t pinBit = (1 << pin);
  uint8_t stateBit = (state << pin);
  switch (port) {
    case 0:
      GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTA_BASE, pinBit, stateBit);
      break;
    case 1:
      GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTB_BASE, pinBit, stateBit);
      break;
    case 2:
      GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTC_BASE, pinBit, stateBit);
      break;
    case 3:
      GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTD_BASE, pinBit, stateBit);
      break;
    case 4:
      GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTE_BASE, pinBit, stateBit);
      break;
    case 5:
      GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, pinBit);
      GPIOPinWrite(GPIO_PORTF_BASE, pinBit, stateBit);
      break;
  }

  return 0;
}
