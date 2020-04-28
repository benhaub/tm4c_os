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
/* PIOSC clock is default. See Pg. 220 and Pg. 256-257 */
static void systick_init() {
	/* Make sure systick is disabled for initialization */
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = CLOCK_FREQ;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x1;
	return;
}
/*
 * 1ms delay
 */
void delay_1ms() {
	systick_init();
	NVIC_ST_RELOAD_R = CLOCK_TICK;
	NVIC_ST_CURRENT_R = 0;
	while(!(NVIC_ST_CTRL_R & (1 << 16)));
	return;
}
/*
 * Start a system clock tick with interrupts enabled.
 */
void start_clocktick() {
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = CLOCK_TICK;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x7;
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
 * Write values in in ram from starting from saddr and ending at eaddr into
 * flash memory that starts at faddr. You can not write more than 1KB at a time.
 * Returns 0 on success, -1 on error.
 */
int write_flash(void *saddr, void *eaddr, void *faddr) {
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

/*TODO:
 * Test
 */
void erase_flash(word *pageaddr) {
/* Align the flash address to the nearest 1KB boundary */
	FLASH_FMA_R = pageaddr & ~0x3FF;
/* Erase the 1KB block of flash starting at pageaddr. */
  FLASH_FMC_R |= FLASH_FMC_WRKEY | FLASH_FMC_ERASE;
  while(FLASH_FMC_R);
  return;
}

/*
 * Protect flash in 2KB blocks up to the flash address given
 */
/*TODO
 * Description is inaccurate, function is not that useful, and untested. You
 * should be able to protect invidual pages.
 */
void protect_flash(int numpages) {
	int protbits = 0;
	int i = 0;
	for(i = numpages - 1; i >= 0; i--) {
		protbits |= (1 << i);
	}
	if(0 == protbits) {
		protbits++;
	}
	FLASH_FMPPE0_R &= ~protbits;
}
/**************************************UART***********************************/
/*TODO:*/
