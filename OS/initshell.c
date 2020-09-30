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

int smain(void) __attribute__((section(".text.smain")));

/*
 * Got nothing to do? How about counting to 10 million?
 */
void count() {
  for(int i = 0; i < 10E6; i++);
  return;
}

/**
 * The led should be purple when this test is done.
 * First the parent turns on the green led, then forks NPROC processes. The
 * children all turn off the green led and then exit while the parent waits
 * for them. When all the children have exited, the parent turns on the red
 * led and then exits.
 */
void forktest() {
	led(LED_GREEN, LED_ON);
	int i;
	int pids[NPROC];
	for(i = 0; i < NPROC; i++) {
		pids[i] = fork();
		if(-1 == pids[i]) {
			exit(EXIT_FAILURE);
		}
		if(NULLPID == pids[i]) {
			/* Child process */
      count();
			led(LED_GREEN, LED_OFF);
			exit(EXIT_SUCCESS);
			led(LED_GREEN, LED_ON);
		}
		else {
			/* Parent process. */
			led(LED_BLUE, LED_ON);
		}
	}
	for(i = 0; i < NPROC; i++) {
		wait(pids[i]);
	}
	led(LED_RED, LED_ON);
}

/**
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
      printf("Flash write failed\n\r");
			return;
		}
		else {
      i++;
		}
	}
/* Make another write in the middle of the page. Make sure the first write is */
/* still in flash, and make sure the new write worked properly. */
  tw2.first = 0x11191555;
  tw2.second = 0x8675309;
  tw2.third = 0xBADDAD;
  flash(&tw2, &tw2 + 1, faddr + 40);
  i = 0;
  while((word)(faddr + i) < (word)faddr + sizeof(tw)) {
    if(*(raddr + i) != *(faddr + i)) {
      printf("Flash write failed\n\r");
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
      printf("Flash write failed\n\r");
      return;
    }
    else {
      i++;
    }
  }
}

/**
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
  printf("255 in hex is %x and 45 as an integer is %i\n\r", 0xFF, 45);
  printf("%x, %i, %x, %i\n\r", 0xCAF, 142, 0xFFFFFFFF, 2147483647);
  return 0;
}

/**
 * Test The OSs ability to detect stack overflow. The process should exit
 * before it's able to turn on the green LED.
 */
void stack_overflow() {
/* Allocate an array that is greater than STACK_SIZE */
  int big_array[384]; //1.5KB
/* Fork a process so that initshell doesn't take the hit. */
  int pid = fork();
  if(0 != pid) {
    for(int i = 0; i < 40; i++) {
      big_array[i] = 0;
    }
    led(LED_GREEN, LED_ON);
  }
}
/**
 * Shell main. The first user program run by the kernel after reset.
 */
int smain() {
/* Commented out to reduce flash writes while testing. */
	//wrflash();
  stringtest();
  forktest();
  stack_overflow();
	return 0;
}
