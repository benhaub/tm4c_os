/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	init.c
 * Synopsis	:	OS entry point. Boot loader loads the address from the first
 * 						entry of the vector table, then looks for the entry point in the
 * 						next word
 * Date			:	May 6th, 2019
 *****************************************************************************/
#include <init.h>
#include <tm4c123gh6pm.h>
#include <hw.h>

int main() {
	led_init();
	led_ron();
	delay_1ms();
	led_bloff();
	return 0;
}
