/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    hw.c                                                            
 * @date    May 16th, 2019                                                
 * @details \b Synopsis: \n Hardware peripheral calls for TM4C123                         
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>
#include <types.h>

/******************************SYSTEM CONTROL*********************************/

/**
 * @brief
 *   Calculates the clock frequency
 *
 * Determines the frequency which will be generated given the following params
 * Frequency is returned in units of Hz.
 *
 * @param cs_config
 *   Must be set according to @sa clocksource_config_t
 * @return
 *   0 on failure due to invalid clock settings, or the non-zero frequency on
 *   success.
 */
static float determine_frequency(struct clocksource_config_t cs_config) {
  switch(cs_config.oscsrc) {
    case 0:
      if(cs_config.sysdiv) {
        if(cs_config.use_pll) {
          return 200.0f*1E6 / cs_config.sysdiv;
        }
        else {
          return (float)MAIN_OSC_FREQ / cs_config.sysdiv;
        }
      }
      else if(cs_config.sysdiv2) {
        if(cs_config.use_pll) {
          if(cs_config.div400) {
            return 400.0f*1E6 / cs_config.sysdiv2;
          }
          else {
            return 200.0f*1E6 / cs_config.sysdiv2;
          }
        }
        else {
          return MAIN_OSC_FREQ / cs_config.sysdiv2;
        }
      }
      else {
        return (float)MAIN_OSC_FREQ;
      }
    case 1:
      if(cs_config.sysdiv) {
        if(cs_config.use_pll) {
          return 200.0f*1E6 / cs_config.sysdiv;
        }
        else {
          return 16.0f*1E6 / cs_config.sysdiv;
        }
      }
      else if(cs_config.sysdiv2) {
        if(cs_config.use_pll) {
          if(cs_config.div400) {
            return 400.0f*1E6 / cs_config.sysdiv2;
          }
          else {
            return 200.0f*1E6 / cs_config.sysdiv2;
          }
        }
        else {
          return 16.0f*1E6 / cs_config.sysdiv2;
        }
      }
      else {
        return 16.0f*1E6;
      }
    case 2:
      if(cs_config.sysdiv) {
        return 16.0f*1E6 / cs_config.sysdiv;
      }
      else if(cs_config.sysdiv2) {
        return 16.0f*1E6 / cs_config.sysdiv2;
      }
      else {
        return 0;
      }
    case 3:
      if(cs_config.sysdiv) {
        return 0.03f*1E6 / cs_config.sysdiv;
      }
      else if(cs_config.sysdiv2) {
        return 0.03f*1E6 / cs_config.sysdiv2;
      }
      else {
        return 0.03f*1E6;
      }
    case 4:
      if(cs_config.sysdiv) {
        return 0.032768f*1E6 / cs_config.sysdiv;
      }
      else if(cs_config.sysdiv2) {
        return 0.032768f*1E6/ cs_config.sysdiv2;
      }
      else {
        return 0.032768f*1E6;
      }
    default:
      return 0;
  }
}
/**
 * @brief
 *   Set the clock source to be used as SysClk.
 * @param cs_config
 *   @see clocksource_config_t
 * @param SysClk
 *   The frequency in Hz that the clock has been set to is stored on return.
 * @return 0 on success, -1 otherwise. As an added check, you may also verify
 * that the frequency you expected is the same one returned by this function.
 * @pre
 *   If the EEPROM is being used, there must not be an EEPROM operation in
 *   progress.
 */
int set_clocksource(struct clocksource_config_t cs_config, float *SysClk) {
/* Zero out the RCC and RCC2 registers. */
  if(cs_config.sysdiv && cs_config.sysdiv2) {
/* Clocks can not be set using both sysdiv and sysdiv2 */
    return -1;
  }
  if(cs_config.use_pll && cs_config.sysdiv2 > 0 && cs_config.sysdiv2 < 2) {
/* You can't divide by less than MINSYSDIV when using the PLL. */
    return -1;
  }
  if(cs_config.use_pll && cs_config.sysdiv > 0 && cs_config.sysdiv < 2) {
/* You can't divide by less than MINSYSDIV when using the PLL. */
    return -1;
  }
  if(cs_config.div400 && cs_config.sysdiv2 < 5) {
/* sysdiv2 must be at least 5 when using div400 */
    return -1;
  }
  if(cs_config.div400 && !cs_config.use_pll) {
/* div400 is only valid when the pll is being used. */
    return -1;
  }
  else if((SYSCTL_RCGCEEPROM_R & 0x1) && EEPROM_EEDONE_R & 0x1) {
    //You can't adjust clocks while an EEPROM operation is in progress.
    return -1;
  }
  if(SysClk != NULL) {
    *SysClk = determine_frequency(cs_config);
  }
  //Set the RCC registers back to default values.
  SYSCTL_RCC_R = (0x78e3ad1);
  SYSCTL_RCC2_R = (0x7c06810);
  switch(cs_config.oscsrc) {
    case 0:
      SYSCTL_RCC_R |= 0x1; //Disable the main oscillator
      SYSCTL_RCC_R &= ~(0x1F << 6); //Clear, then set XTAL
      SYSCTL_RCC_R |= (0x15 << 6);
/* Set the clocksource in RCC2 if sysdiv2 is being used, otherwise set it in */
/* RCC. */
      if(cs_config.sysdiv2) {
        SYSCTL_RCC2_R &= ~(7 << 4);
      }
      else {
        SYSCTL_RCC_R &= ~(7 << 4);
      }
      if(cs_config.use_pll) {
        SYSCTL_RCC_R &= ~(1 << 11);
        SYSCTL_RCC_R &= ~(1 << 13); //Power up the PLL
      }
      else {
        SYSCTL_RCC_R |= (1 << 11);
      }
      SYSCTL_RCC_R &= ~0x1; //Enable the main oscillator
/* Check if the MOSC has failed */
      if(SYSCTL_RIS_R & (1 << 3)) {
        SYSCTL_MISC_R |= (1 << 3);
        return -1;
      }
      while(!(SYSCTL_RIS_R & (1 << 8))); //Wait for the MOSC to lock
      SYSCTL_MISC_R |= (1 << 8); //Clear the interrupt.
      break;
    case 1:
      if(cs_config.sysdiv2) {
        SYSCTL_RCC2_R |= (1 << 4);
      }
      else {
        SYSCTL_RCC_R |= (1 << 4);
      }
      if(cs_config.use_pll) {
        SYSCTL_RCC_R &= ~(1 << 11);
        SYSCTL_RCC_R &= ~(1 << 13); //Power up the PLL
      }
      else {
        SYSCTL_RCC_R |= (1 << 11);
        SYSCTL_RCC_R |= (1 << 13); //Power down the PLL
      }
      break;
/* PLL can only be used for PIOSC and MOSC. */
    case 2:
      if(cs_config.sysdiv2) {
        SYSCTL_RCC2_R |= (2 << 4);
      }
      else {
        SYSCTL_RCC_R |= (2 << 4);
      }
      SYSCTL_RCC_R |= (1 << 11); //oscsrc does not pass through PLL
      break;
    case 3:
      if(cs_config.sysdiv2) {
        SYSCTL_RCC2_R |= (3 << 4);
      }
      else {
        SYSCTL_RCC_R |= (3 << 4);
      }
      SYSCTL_RCC_R |= (1 << 11);
      break;
/* This oscsrc is only available on RCC2. */
    case 4:
      SYSCTL_RCC_R |= (1 << 11);
      SYSCTL_RCC2_R |= (7 << 4);
      break;
    default: /* Invalid oscillator source */
      return -1;
  }
/* These settings use RCC */
  if(cs_config.sysdiv > 1 && cs_config.sysdiv <= 16) {
    SYSCTL_RCC_R |= (1 << 22); //Set USESYSDIV
    SYSCTL_RCC_R &= ~(0xF << 23); //Clear and set sysdiv.
    SYSCTL_RCC_R |= ((cs_config.sysdiv - 1) << 23);
    SYSCTL_RCC2_R &= ~(1 << 31); //Do not use RCC2
  }
/* These parameters require the use of RCC2. */
  else if(cs_config.sysdiv2 > 1 && cs_config.sysdiv2 <= 128) {
    SYSCTL_RCC_R |= (1 << 22); //Set USESYSDIV
    SYSCTL_RCC2_R |= (1 << 31); //RCC2 overrides RCC
    SYSCTL_RCC2_R &= ~(0x3F << 23); //Clear and set sysdiv.
    if(cs_config.div400) {
      SYSCTL_RCC2_R |= (1 << 30);
      //Bit field encodings are offset by 3 for div400.
      SYSCTL_RCC2_R |= ((cs_config.sysdiv2 - 3) << 23); //SYSDIV2
    }
    else {
      SYSCTL_RCC2_R &= ~(1 << 30);
      SYSCTL_RCC2_R |= ((cs_config.sysdiv2 - 1) << 23); //SYSDIV2
    }
    if(cs_config.use_pll) {
      SYSCTL_RCC2_R &= ~(1 << 11);
      SYSCTL_RCC2_R &= ~(1 << 13); //Power up the PLL
    }
    else {
      SYSCTL_RCC2_R |= (1 << 11);
      SYSCTL_RCC2_R |= (1 << 13); //Power down the PLL
    }
  }
/* Not using sysdiv */
  else {
    SYSCTL_RCC_R &= ~(1 << 22);
  }

  /* As a final step, if the PLL was used, allow time for it to lock the */
  /* frequency in */
  if(cs_config.use_pll) {
    while(!(SYSCTL_RIS_R & (1 << 6)));
    SYSCTL_MISC_R |= (1 << 6); //Clear the interrupt.
  }
  return 0;
}

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
    NVIC_ST_RELOAD_R = (SysClkFrequency / 1000) * period;
  }
  else {
    NVIC_ST_RELOAD_R = (SysClkFrequency / 4000) * period;
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
	NVIC_ST_RELOAD_R = (SysClkFrequency / 1000);
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
  if((uint32_t)eaddr - (uint32_t)saddr > 1*KB) {
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
    brd = (float)PIOSC_FREQ / (8 * baud);
  }
  else {
    brd = (float)PIOSC_FREQ / (16 * baud);
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
 *
 * Initialise a Synchronous Serial Interface master on GPIO Port A by following
 * the procedure on Pg. 965 of the datasheet. This procedure will not complete
 * and return -1 if port control for PA[5:2] has been changed from the POR
 * default.
 *
 * @param protocol
 *   The protocol to use.
 *   \b 0 for Freescale SPI \n
 *   \b 1 for Texas Instruments SSI \n
 *   \b 2 for Microwire SSI \n
 * @param ds
 *   The data size for each frame. Anywhere from 0x3 (4-bit data) to 0xF
 *   (16-bit data) is allowed.
 * @param drxr
 *   The drive strength for the pin. \n
 *   \b 2 for 2mA \n
 *   \b 4 for 4mA \n
 *   \b 8 for 8mA
 * @param ff
 *   Frame format for the SPI lines \n
 *   \b 0 to hold the clock low during idle. \n
 *     Data is captured on the rising edge of the clock \n
 *   \b 1 same as \b 0, but data is captured on the falling edge of the clock. \n
 *   \b 2 to hold the clock high during idle. Data is caputured on the falling \n
 *     edge of the clock
 *   \b 3 same as \b 2, but data is captured on the rising edge.
 * @note
 *   PA2 - clock \n
 *   PA3 - frame signal \n
 *   PA4 - MOSI. (Driven by master, received by slave) \n
 *   PA5 - MISO. (Driven by slave, received by master)
 */
int ssi0_init_master(int protocol, int ds, int drxr, int ff) {
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
  GPIO_PORTA_DIR_R |= (1 << 4);
  GPIO_PORTA_DIR_R &= ~(1 << 5);
  switch (drxr) {
    case 2: GPIO_PORTA_DR2R_R |= 0x3C;
            break;
    case 4: GPIO_PORTA_DR4R_R |= 0x3C;
            break;
    case 8: GPIO_PORTA_DR8R_R |= 0x3C;
            break;
    default: return -1;
  }
/* Disable SSI0 for initialization. */
  SSI0_CR1_R &= ~(1 << 1);
/* Configure the SSI peripheral as master. */
  SSI0_CR1_R &= ~(1 << 2);
/* The clock source is the system clock. See init() in main.c or the value of */
/* SysClkFrequency. */
  SSI0_CC_R &= ~0xF;
/* Divide the SysClk by 2. The formula is SSInClk=SysClk/(CPSDVSR * (1 + SCR) */
  SSI0_CPSR_R |= 0x2; //DVSR
  SSI0_CR0_R &= ~(0xFF << 8); //SCR
/* SSInClk is now SysClk/2. Define the protocol. */
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
/*Frame select */
  switch(ff) {
    case 0: SSI0_CR0_R &= ~(1 << 7);
            SSI0_CR0_R &= ~(1 << 6);
            break;
    case 1: SSI0_CR0_R |= (1 << 7);
            SSI0_CR0_R &= ~(1 << 6);
            break;
    case 2: SSI0_CR0_R &= ~(1 << 7);
            SSI0_CR0_R |= (1 << 6);
            GPIO_PORTA_PUR_R |= (1 << 2);
            GPIO_PORTA_ODR_R |= (1 << 2);
            break;
    case 3: SSI0_CR0_R |= (1 << 7);
            SSI0_CR0_R |= (1 << 6);
            GPIO_PORTA_PUR_R |= (1 << 2);
            GPIO_PORTA_ODR_R |= (1 << 2);
            break;
    default: return -1;
  }
/*Data size */
  if(ds <= 0xF && ds >= 0x3) {
      SSI0_CR0_R |= ds;
  }
/* Enable SSI0. */
  SSI0_CR1_R |= (1 << 1);
/* No DMA for now... */
  return 0;
}

/**
 * @brief
 *   Non-blocking SSI data transmission SSI0.
 * @param data
 *   The data to transmit
 * @return
 *   0 if the fifo was empty and the data could be transmitted, -1 otherwise.
 * @todo
 *   Should queue up if the fifo is not empty and transmit it later when it
 *   is.
 */
int ssi0_transmit(uint8_t data) {
/* Check status register to see if the FIFO is empty. */
  if((SSI0_SR_R & (1 << 1))) {
/* Write the data register for transmission. */
      SSI0_DR_R = data;
      return 0;
  }
  else {
    return -1;
  }
}

/**
 * @brief
 *   Receive SSI data
 * @return
 *   The SSI data
 */

uint8_t ssi0_receive() {
  return SSI0_DR_R;
}
/*****************************General Purpose Timer****************************/

gptm_timer_t gptm_timer_init(struct timer_config_t *config) {
  gptm_timer_t timer = NULL;
  //Find an unused timer by first checking if the clock to the module has been
  //enabled. If it has, then timer A is in use.
  if (0 == (SYSCTL_RCGCTIMER_R & 0x1)) {
    if(timer0A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER0_CFG_R;
    }
  }
  else if(0 == (TIMER0_CTL_R & 1<<8)) {
    if(timer0B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER0_CFG_R;
    }
  }
  else if(0 == (SYSCTL_RCGCTIMER_R & 1<<1)) {
    if(timer1A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER1_CFG_R;
    }
  }
  else if(0 == (TIMER1_CTL_R & 1<<8)) {
    if(timer1B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER1_CFG_R;
    }
  }
  else if (0 == (SYSCTL_RCGCTIMER_R & 1<<2)) {
    if(timer2A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER2_CFG_R;
    }
  }
  else if(0 == (TIMER2_CTL_R & 1<<8)) {
    if(timer2B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER2_CFG_R;
    }
  }
  else if (0 == (SYSCTL_RCGCTIMER_R & 1<<3)) {
    if(timer3A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER3_CFG_R;
    }
  }
  else if(0 == (TIMER3_CTL_R & 1<<8)) {
    if(timer3B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER3_CFG_R;
    }
  }
  else if (0 == (SYSCTL_RCGCTIMER_R & 1<<4)) {
    if(timer4A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER4_CFG_R;
    }
  }
  else if(0 == (TIMER4_CTL_R & 1<<8)) {
    if(timer4B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER4_CFG_R;
    }
  }
  else if (0 == (SYSCTL_RCGCTIMER_R & 1<<5)) {
    if(timer5A_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER5_CFG_R;
    }
  }
  else if(0 == (TIMER5_CTL_R & 1<<8)) {
    if(timer5B_init(config)) {
      return timer;
    }
    else {
      timer = &TIMER5_CFG_R;
    }
  }

  return timer;
}

uint8_t timer0A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= 0x1;
  //Wait for the timer to be ready to access
  while (0 == (SYSCTL_PRTIMER_R & 0x1));
  TIMER0_CFG_R = 0x0;
  TIMER0_TAMR_R |= config->periodic ? 0x2 : 0x1;
  if (config->pwm) {
    TIMER0_TAMR_R &= ~(1<<2);
    TIMER0_TAMR_R |= (1<<3);
    if (0 == (TIMER0_TAMR_R & 0x3) || 3 == (TIMER0_TAMR_R & 0x3))
      //pwm mode must set either one-shot or peridic
      return 1;
  }
  TIMER0_TAMR_R |= config->direction<<4;
  TIMER0_TAMR_R |= config->wait_on_trigger<<6;
  TIMER0_TAMR_R |= config->snapshot_mode<<7;

  TIMER0_TAILR_R = config->interval;
  config->instance = 0;
  return 0;
}

uint8_t timer0B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= 0x1;
  TIMER0_CFG_R = 0x0;
  TIMER0_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER0_CTL_R |= (1<<8);
  return 0;
}

uint8_t timer1A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<1);
  TIMER1_CFG_R = 0x0;
  TIMER1_TAMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER1_CTL_R |= 0x1;
  return 0;
}

uint8_t timer1B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<1);
  TIMER1_CFG_R = 0x0;
  TIMER1_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER1_CTL_R |= (1<<8);
  return 0;
}

uint8_t timer2A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<2);
  TIMER2_CFG_R = 0x0;
  TIMER2_TAMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER2_CTL_R |= 1;
  return 0;
}

uint8_t timer2B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<2);
  TIMER2_CFG_R = 0x0;
  TIMER2_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER2_CTL_R |= (1<<8);
  return 0;
}

uint8_t timer3A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<3);
  TIMER3_CFG_R = 0x0;
  TIMER3_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER3_CTL_R |= 1;
  return 0;
}

uint8_t timer3B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<3);
  TIMER3_CFG_R = 0x0;
  TIMER3_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER3_CTL_R |= (1<<8);
  return 0;
}

uint8_t timer4A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<4);
  TIMER4_CFG_R = 0x0;
  TIMER4_TAMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER4_CTL_R |= 1;
  return 0;
}

uint8_t timer4B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<4);
  TIMER4_CFG_R = 0x0;
  TIMER4_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER4_CTL_R |= (1<<8);
  return 0;
}

uint8_t timer5A_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<5);
  TIMER5_CFG_R = 0x0;
  TIMER5_TAMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER5_CTL_R |= 1;
  return 0;
}

uint8_t timer5B_init(struct timer_config_t *config) {
  SYSCTL_RCGCTIMER_R |= (1<<5);
  TIMER5_CFG_R = 0x0;
  TIMER5_TBMR_R |= config->periodic ? 0x2 : 0x1;

  TIMER5_CTL_R |= (1<<8);
  return 0;
}

void gptm_start_timer(gptm_timer_t timer_base, uint8_t timer_instance) {
  if (timer_instance)
    //Timer B
    *(timer_base + 3) |= (1<<8);
  else
    //Timer A
    *(timer_base + 3) |= 0x1;
}

/*
 * @breif
 *   Check the raw interrupt status of the timer to see if it's timed out.
 * @param base
 *   The timer to check
 * @param instance
 *   The timer instance to check.
 * @return
 *   non-zero if the timer has not timed out.
 * @post
 *   The interrupt status is cleared
 */
uint8_t gptm_timeout(gptm_timer_t base, uint8_t timer_instance) {
  if (timer_instance) {
    //Timer B
    if (*(base + 7) & 1<<8) {
      (*(base + 9)) |= 1<<8;
      return 0;
    }
  }
  else {
    //Timer A
    if (*(base + 7) & 0x1) {
      (*(base + 9)) |= 0x1;
      return 0;
    }
  }

  return 1;
}
