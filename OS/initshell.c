/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	initshell.c
 * Synopsis	:	Implements the shell for tm4c_os
 * Date			:	July 10th, 2019
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h>
#include <syscalls.h>

/*
 * Shell main
 */
int smain(void) __attribute__ ((section (".text.smain")));
int smain() {
	led_init();
	led_gron();
	int pid = fork();
	if(0 == pid) {
		/* Child process */
		led_groff();
	}
	led_blon();
	return 0;
}
