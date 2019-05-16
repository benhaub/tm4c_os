/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	init.c
 * Synopsis	:	OS entry point. Boot loader loads the address from the first
 * 						entry of the vector table, then looks for the entry point in the
 * 						next word
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <init.h>

int main() {
	/* You can find which pins are LEDs by seeing the Tiva C Series LaunchPad */
	/* Evaluation Kit User's Manual, Pg.9. The Initializaton process is found */
	/* in the datasheet on Pg.656. */
	//putvec();

	/* Turn the light orange for now to see if we can get here */
	SYSCTL_RCGCGPIO_R |= 0x20;
	/* Dummy instruction to let the clock settle */
	unsigned int dlyclk = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_DIR_R |= (1 << 1);
	GPIO_PORTF_DIR_R |= (1 << 3);
	GPIO_PORTF_AFSEL_R &= ~0x01;
	GPIO_PORTF_DEN_R |= (1 << 1);
	GPIO_PORTF_DEN_R |= (1 << 3);
	GPIO_PORTF_DATA_R |= (1 << 1);
	GPIO_PORTF_DATA_R |= (1 << 3);
	while(1);
	return 0;
}
