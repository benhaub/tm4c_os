/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : hw.c                                                            *
 * Synopsis : Hardware peripheral calls for TM4C123                           *
 * Date     : May 16th, 2019                                                  *
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>
#include <types.h>

/*********************************SYSTICK*************************************/
/* PIOSC clock is default. See Pg. 219 and Pg. 256-257. RCC is left at */
/* default, so what the datasheet refers to as the "System Clock" is the same */
/* as the OSCSRC since the BYPASS is not enabled, nor is SYSDIV. */
void systick_init() {
	/* Make sure systick is disabled for initialization */
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = (SYS_CLOCK_FREQ >> 2);
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x1;
	return;
}
/*
 * Start a system clock tick with interrupts enabled. Interrupts frequencies
 * must be such that the OS has time to complete scheduling and context
 * switching.
 */
void start_clocktick() {
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = CLOCK_TICK*10;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x7;
	return;
}
/*
 * 1ms delay
 */
void delay_1ms() {
	NVIC_ST_RELOAD_R = CLOCK_TICK;
	NVIC_ST_CURRENT_R = 0;
	while(!(NVIC_ST_CTRL_R & (1 << 16)));
	return;
}

/********************************LEDs*****************************************/

/* You can find which pins are LEDs by seeing the Tiva C Series LaunchPad */
/* Evaluation Kit User's Manual, Pg.9. The Initializaton process is found */
/* in the datasheet on Pg.656. */

/*
 * Initialize PortF for led operation. This function must be run before the LEDs
 * can be used.
 */
void led_init() {
	SYSCTL_RCGCGPIO_R |= (1 << 5); //Enable port f
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

/*
 * Turn the red led on
 */
void led_ron() {
	GPIO_PORTF_DATA_R |= (1 << 1);
	return;
}

/*
 * Turn off red led
 */
void led_roff() {
	GPIO_PORTF_DATA_R &= ~(1 << 1);
	return;
}

void led_gron() {
	GPIO_PORTF_DATA_R |= (1 << 3);
	return;
}

void led_groff() {
	GPIO_PORTF_DATA_R &= ~(1 << 3);
	return;
}

void led_blon() {
	GPIO_PORTF_DATA_R |= (1 << 2);
	return;
}

void led_bloff() {
	GPIO_PORTF_DATA_R &= ~(1 << 2);
	return;
}

/******************************Flash Memory***********************************/

/*
 * Follows the procedure on Pg. 532, datasheet.
 * Write values in ram from starting from saddr and ending at eaddr into
 * flash memory that starts at faddr. This function allows you to write a
 * maximum of 1KB per call. For successive block writes, make multiple calls.
 * This call requires more than 1KB of stack space. User programs
 * should not call this function directly; use the system call instead.
 * Returns 0 on success, -1 on error.
 */
int write_flash(void *saddr, void *eaddr, void *faddr) {
  if((word)faddr <= 0x1000) {
    while(1);
  }
  if((word)eaddr - (word)saddr > 1*KB) {
    return -1;
  }
/* Align the flash address to the nearest 1KB boundary */
	FLASH_FMA_R = ((word)faddr & ~(0x3FF));
/* If the write can't be done with one set of buffer registers, then set the */
/* cflag to re-fill the buffer regs and continue writing. */
	int cflag = 0;
/* current address being written to flash copy */
	word *curraddr = (word *)saddr;
/* The fpage_os is the offset of FLASH_FMA_R where the flash writes begin at.*/
/* This address must be word aligned. */
	word fpage_os = (word)faddr & 0xFC;
/* Since 0's can not be programmed back to a 1. We have to copy and erase */
/* flash before making the write. */
	word fcopy[256]; /* 1KB of space */
	int i, j;
	for(i = 0; i < 256; i++) {
		fcopy[i] = *((word *)FLASH_FMA_R + i);
	}
/* Write the new data into the copy we've obtained. */
  if(fpage_os != 0) {
    i = (fpage_os >> 2);
  }
  else {
    i = 0;
  }
/* Write in the new values into the copy we've obtained. */
  while(curraddr < (word *)eaddr) {
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
	while((FLASH_FMA_R + i*4) < (FLASH_FMA_R + 1*KB)) {
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

/*
 * Erase the 1KB flash page that contains the address pageaddr.
 */
void erase_flash(word pageaddr) {
/* Align the flash address to the nearest 1KB boundary */
	FLASH_FMA_R = pageaddr & ~0x3FF;
/* Erase the 1KB block of flash starting at pageaddr. */
  FLASH_FMC_R |= FLASH_FMC_WRKEY | FLASH_FMC_ERASE;
  while(FLASH_FMC_R);
  return;
}

/*
 * Permanently protect the flash page given by pageno from being modified by a
 * flash write. Page 0 will protect flash memory from 0x0 to 0x7FF. 1 will
 * protect from 0x800 to 0xFFF, etc.
 * @return 
 *   -1 on failure, 0 on success.
 */
int protect_flash(int pageno) {
  if(pageno < 0 || pageno > 128) {
    return -1;
  }
  if(pageno < 32) {
    FLASH_FMPPE0_R &= (1 << pageno);
  }
  else if(pageno <  64) {
    FLASH_FMPPE0_R &= (1 << (pageno % 32));
  }
  else if(pageno < 96) {
    FLASH_FMPPE0_R &=(1 << (pageno % 32));
  }
  else {
    FLASH_FMPPE0_R &= (1 << (pageno % 32));
  }
  return 0;
}
/***********************************UART**************************************/

/*
 * Initialize uart module 1 to 8N1. Follows the initialization procedure on
 * Pg. 902 of the data sheet. PB0 and PB1 are used for RX and TX respectively.
 * 2mA and default slew are rate are used. Only PB1 TX is enabled since this.
 * uart is used for the kernel and user to print output.
 * @param baud
 *   The baud rate to be used for the module
 */
void uart1_init(unsigned int baud) {
/* baud rate divisor. */
  float brd;
/* integer and float parts of the baud rate divisor. */
  int ibrd, fbrd;
/* Enable run mode for uart module 1. */
  SYSCTL_RCGCUART_R |= (1 << 1);
/* Enable run mode for GPIO Port B module (GPIOPB). */
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
  if(0 != (GPIO_PORTB_AFSEL_R & (1 << 1))) {
    return;
  }
  GPIO_PORTB_AFSEL_R |= (1 << 1); //UART1 alt function for PB1.
  GPIO_PORTB_PCTL_R |= (1 << 4); //Transmit function for PB1.
/* Initialize GPIOPB1. Pg. 656 initialization procedure. */
  GPIO_PORTB_DEN_R |= (1 << 1); //Digital input/output, as opposed to analog.
  GPIO_PORTB_ODR_R &= ~(1 << 1); //Open drain.
/* Continue on with UART init by first disabling it during setup. */
  UART1_CTL_R &= ~0x1;
/* Use the system clock and generate baud rates. 16 is the divisor */
/* of the system clock for the UART clock obtained from the value of the HSE */
/* bit in the UART control register. */
  brd = (float)SYS_CLOCK_FREQ / (16 * baud);
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
}

/*
 * Write a character to the FIFO and initiate a transfer.
 * @param data
 *   8 bit data to transmit.
 * returns 0 if the data was written to the FIFO, -1 otherwise. The return
 * value does NOT indicate a sucessful transmission, only that data was
 * transferred.
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
  if(UART1_FR_R & (1 << 7)) {
    return -1;
  }
  return 0;
}

/*************************************SSI*************************************/

/*
 * Initialise a Synchronous Serial Interface master on GPIO Port A by following
 * the procedure on Pg. 965 of the datasheet. This procedure will not complete
 * and return -1 if port control for PA[5:2] has been changed from the POR
 * default.
 * @param protocol
 *   0 for Freescale SPI
 *   1 for Texas Instruments SSI
 *   2 for Microwire SSI
 * @param ds
 *   The data size for each frame. Anywhere from 0x3 (4-bit data) to 0xF
 *   (16-bit data) is allowed.
 * @param drxr
 *   The drive strength for the pin.
 *   2 for 2mA
 *   4 for 4mA
 *   8 for 8mA
 */
int ssi0_init_master(int protocol, int ds, int drxr) {
/* Activate the SSI peripheral clock. */
  SYSCTL_RCGCSSI_R |= 0x1;
/* Enable and provide a clock to GPIO Port A. */
  SYSCTL_RCGCGPIO_R |= 0x1;
/* There is no need to explicitely set the alternate function bits since */
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
  GPIO_PORTA_AFSEL_R |= 0x3C;
  GPIO_PORTA_DEN_R |= 0x3C;
  switch (drxr) {
    case 2: GPIO_PORTA_DR2R_R |= 0x3C;
            break;
    case 4: GPIO_PORTA_DR4R_R |= 0x3C;
            break;
    case 8: GPIO_PORTA_DR8R_R |= 0x3C;
            break;
    default: return -1;
  }
  //TODO: What are we doing with these?
  //GPIO_PORTA_PUR_R |= (1 << 3);
  //GPIO_PORTA_ODR_R |= (1 << 3);
/* Disable SSI0 for initialisation. */
  SSI0_CR1_R &= ~(1 << 1);
/* Configure the SSI peripheral as master. */
  SSI0_CR1_R &= ~(1 << 2);
/* The clock source is the system clock, which for tm4c_os is the same as */
/* PIOSC. */
  SSI0_CC_R &= ~0x1;
/* Divide the SysClk by 2. The formula is BR=SysClk/(CPSDVSR * (1 + SCR) */
/* Where BR is the Bit Rate, SysClk is 16MHz. */
  SSI0_CPSR_R |= 0x2; //DVSR
  SSI0_CR0_R &= ~(0xFF << 8); //SCR
/* BR is now 8MHz. Define the protocol. */
  switch(protocol) {
    case 0: SSI0_CR0_R &= ~(0x3 << 4); //Freescale SPI
            SSI0_CR0_R &= ~(1 << 6);
            SSI0_CR0_R &= ~(1 << 7);
            break;
    case 1: SSI0_CR0_R |= (1 << 4); //TI SSI
            break;
    case 2: SSI0_CR0_R |= (2 << 4); //Microwire
            break;
    default: return -1;
  }
  if(ds <= 0xF && ds >= 0x3) {
      SSI0_CR0_R |= ds;
  }
/* Enable SSI0. */
  SSI0_CR1_R |= (1 << 1);
  return 0;
}

/*
 * Transmit data on SSI0
 */
void ssi0_transmit(int data) {
/* Check Raw Interrupt Status to see if the FIFO is empty. */
  if((1 << 3) == (SSI0_RIS_R & (1 << 3))) {
/* Write the data register for transmission. */
      SSI0_DR_R = data;
  }
}
