/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : initshell.c                                                     *
 * Synopsis : Implements the shell for tm4c_os. For now, it's being used to   *
 *            write functions to test the operating system.                   *
 * Date     : July 10th, 2019                                                 *
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h> /* For led functions */
#include <proc.h> /* For NULLPID, exit macros */
#include <syscalls.h>
#include <mem.h> /* For flash address macros */
#include <cstring.h> /* For testing cstring api */

/* The led should be purple when this test is done.
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
/*TODO:
 * Need to make sure that forked processes have all the parents stack
 * information.
 */
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
/*TODO:
 * Need a second fork test to test how the OS handles the maximum number of
 * processes.
 */

/* 
 * This function tests reading and writing flash by writing the testwrite
 * struct into flash memory, and then reading it back and comparing the
 * values.
 */
void wrflash() {
  int i = 0;

	struct testwrite {
		int first;
		int second;
		int third;
	}tw, tw2;

	tw.first = 0xDEADBEEF;
	tw.second = 0xCAFEBABE;
	tw.third = 0xC0FFEE;

	word *faddr = (word *)(KFLASHPGS*FLASH_PAGE_SIZE); /* flash address */
	word *raddr = (word *)&tw; /* ram address */

	flash(&tw, &tw + 1, faddr);
/* Compare the values at each address of flash and ram to see if they match */
	while((word)(faddr + i) < (word)faddr + sizeof(tw)) {
		if(*(raddr + i) != *(faddr + i)) {
			return;
		}
		else {
      i++;
		}
	}
/* Make another write in the middle of the page. Make sure the first write is */
/* still in flash, and make sure the new write work properly. */
  tw2.first = 0x11191555;
  tw2.second = 0x8675309;
  tw2.third = 0xBADDAD;
  flash(&tw2, &tw2 + 1, faddr + 40);
  i = 0;
  while((word)(faddr + i) < (word)faddr + sizeof(tw)) {
    if(*(raddr + i) != *(faddr + i)) {
      return;
    }
    else {
      i++;
    }
  }
  raddr = (word *)&tw2;
  i = 0;
  while((word)(faddr + 40 + i) < (word)(faddr + 40)  + sizeof(tw)) {
    if(*(raddr + i) != *(faddr + 40 + i)) {
      return;
    }
    else {
      i++;
    }
  }
}

/*
 * Tests all the functions in cstring.c
 */
int stringtest() {
  char *str1; char *str2;
  str1 = "string1";
  str2 = "string2";
  char str3[15];
  struct context ctx;
  struct context ctx2;

  if(strlen(str1) != 7) {
    return -1;
  }
  if(strlen(str2) != 7) {
    return -1;
  }
  if(strncmp(str1, "string1", strlen(str1)) != 0) {
    return -1;
  }
  strncpy(str3, str1, strlen(str1));
  strncat(str3, str2, strlen(str2));
  if(strncmp(str3, "string1string2", strlen("string1string2")) != 0) {
    return -1;
  }
  memset(str3, 0, strlen(str3));
  memset(&ctx, 0, sizeof(struct context));
  memcpy(&ctx, &ctx2, sizeof(struct context));
  return 0;
}

/*
 * Shell main. The first user program run by the kernel after reset.
 */
int smain(void) __attribute__((section(".text.smain")));
int smain() {
/* Commented out to reduce flash write rates while testing. */
	//wrflash();
  stringtest();
  uart1_tchar('A');
	forktest();
	return 0;
}
