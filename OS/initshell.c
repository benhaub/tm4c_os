/******************************************************************************
 * Authour	:	Ben Haubrich
 * File			:	initshell.c
 * Synopsis	:	Implements the shell for tm4c_os
 * Date			:	July 10th, 2019
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h> /* For led functions */
#include <proc.h> /* For NULLPID, exit macros */
#include <syscalls.h>

/*
 * Shell main. The first user program run by the kernel after reset.
 */
int smain(void) __attribute__ ((section (".text.smain")));
int smain() {
	led_init();
	led_gron();
/* Temporary test of system calls and fork(). */
	int pid = fork();
	if(NULLPID == pid) {
		/* Child process */
		led_groff();
		exit();
	}
	else {
		/* Parent process. */
		led_blon();
		wait(pid);
		led_bloff();
		led_ron();
		exit();
	}
	return 0;
}
