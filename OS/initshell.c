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
int smain() {
	led_init();
	led_gron();
	/* Temporary test of system calls and fork(). */
	int i;
	int pids[3];
	for(i = 0; i < 3; i++) {
		pids[i] = fork();
		if(-1 == pids[i]) {
			exit(EXIT_FAILURE);
		}
		if(NULLPID == pids[i]) {
			/* Child process */
			led_groff();
			exit(EXIT_SUCCESS);
		}
		else {
			/* Parent process. */
			led_blon();
		}
	}
	for(i = 0; i < 3; i++) {
		wait(pids[i]);
	}
	exit(EXIT_SUCCESS);
	return 0;
/*TODO:
 * Let's get a file system up and running in flash, then start making small
 * commands for it like cd and ls. Then for some programs we could just do
 * some things that test the OS.
 */
}
