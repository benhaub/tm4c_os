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

/* The light should be purple when this test is done.
 * First the parent turns on the green led, then forks 4 processes. The
 * children all turn off the green led and then exit while the parent waits
 * from them. When all the children have exited, the parent turns on the red
 * led and then exits.
 */
void forktest() {
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
			led_gron();
		}
		else {
			/* Parent process. */
			led_blon();
		}
	}
	for(i = 0; i < 3; i++) {
		wait(pids[i]);
	}
	led_ron();
	exit(EXIT_SUCCESS);
}
/*
 * Shell main. The first user program run by the kernel after reset.
 */
int smain(void) __attribute__((section(".text.smain")));
int smain() {
	forktest();
	return 0;
}
