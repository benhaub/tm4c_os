/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			:	hw.c																														*
 * Synopsis	:	Hardware peripheral calls for TM4C123														*
 * Date			:	May 16th, 2019																									*
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <mem.h>

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
 * Write value into flash starting at saddr and ending at eaddr.
 * Returns 0 on success, -1 on error.
 */
int write_flash(void *saddr, void *eaddr) {
/* Check for word alignment */
	if(((word)((word *)saddr) & 0x0000007F) > 0) {
		return -1;
	}
/* If the write can't be done with one set of buffer registers, then set the */
/* cflag to continue writing. */
	int cflag = 0;
/* Transfer value to the write buffer */
	word *curraddr = (word *)saddr;
	word nextaddr = 0x0;
Continue:
	while(curraddr <= (word *)eaddr) {
		*(&FLASH_FWBN_R + nextaddr) = *curraddr;
		nextaddr += 0x4;
		curraddr = curraddr + nextaddr;
		if(nextaddr > 0x17C) {
			cflag = 1;
		}
	}
	FLASH_FMA_R = (word)((word *)saddr);
/* Initiate the write sequence */
	FLASH_FMC2_R |= (BOOTKEY << 16);
	FLASH_FMC2_R |= 1;
/* Wait for WRBUF to clear */
	while(FLASH_FMC2_R & 1);
/* Load the buffer registers again if we need to */
	if(1 == cflag) {
		cflag = 0;
		nextaddr = 0x0;
		goto Continue;
	}
	return 0;
}

/* Protect flash in 2KB blocks up to the flash address given */
void protect_flash(word flashaddr) {
	int protbits = 0;
	int i = 0;
	for(i = flashaddr / FLASH_PAGE_SIZE; i > 0; i--) {
		protbits += (1 << i);
	}
	if(0 == protbits) {
		protbits++;
	}
	FLASH_FMPPE0_R &= ~protbits;
}
