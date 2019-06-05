/******************************************************************************
 * Authour	:	Ben Haubrich																										*
 * File			:	hw.c																													*
 * Synopsis	:	Hardware peripheral calls for TM4C123														*
 * Date			:	May 16th, 2019																									*
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
/*********************************SYSTICK*************************************/
void systick_init() {
	/* Make sure systic is disabled for initialization */
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x1;
	return;
}
void delay_1ms() {
	systick_init();
	NVIC_ST_RELOAD_R = 16000;
	while(!(NVIC_ST_CTRL_R & (1 << 16)));
	led_roff();
	led_blon();
	led_gron();
	int whereami = 2;
	//int *ret = (int *)0x000012b0;
	//*ret = 0x00000081;
	return;
}
void start_clocktick() {
	systick_init();
	NVIC_ST_CTRL_R |= (1 << 0);
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
