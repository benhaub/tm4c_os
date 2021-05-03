/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    userspace_testing.c                                                     
 * @date    July 10th, 2019                                                 
 * @details \b Synopsis: \n smain is the first user program called by the OS.
 *          This file tests the OS from non-privledged (non-kernel mode) mode
 *          software.
 *****************************************************************************/
#include <tm4c123gh6pm.h>
#include <hw.h> /* For led functions */
#include <proc.h> /* For NULLPID, exit macros */
#include <syscalls.h>
#include <mem.h> /* For flash address macros */
#include <cstring.h> /* For testing cstring api */

int smain(void) __attribute__((section(".text.smain")));

/**
 * @brief
 *   Got nothing to do? How about counting to 10 million?
 */
void count() {
  for(int i = 0; i < 10E6; i++);
  return;
}

/**
 * @brief
 *   The led should be purple when this test is done.
 *   First the parent turns on the green led, then forks NPROC processes. The
 *   children all turn off the green led and then exit while the parent waits
 *   for them. When all the children have exited, the parent turns on the red
 *   led and then exits.
 */
void forktest() {
	led(LED_GREEN, LED_ON);
	int i;
  pid_t pids[NPROC+10];
	for(i = 0; i < NPROC+10; i++) {
		pids[i] = fork();
		if(0 == pids[i]) {
			//Deliberately be bad and do nothing if we can't fork a process.
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
	for(i = 0; i < NPROC+10; i++) {
		if(-1 == wait(pids[i])) {
      led(LED_BLUE, LED_OFF);
    }
	}
	led(LED_RED, LED_ON);
}

/**
 * @brief
 *   Tests all the functions in cstring.c
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
 * @brief
 *   Test The OSs ability to detect stack overflow. The process should exit
 *   before it's able to turn on the green LED.
 *TODO:
 * Stack over flow is killing userinit!
 */
int stack_overflow() {
  int pid = fork();
  if(NULLPID == pid) {
/* Allocate an array that is greater than STACK_SIZE */
    int big_array[384]; //1.5KB
/* Call wait to force a write to the stack. The array declaration only moves */
/* the stack pointer. Nothing has been written to cause a memory fault yet. */
    wait(NULLPID);
    led(LED_GREEN, LED_ON);
    exit(EXIT_SUCCESS);
  }
  else if(0 == pid) {
    return -1;
  }
  return wait(pid);
}

/**
 * @brief
 *   Try to write to memory we don't own. If the test passes, the child process
 *   should not turn on the green LED.
 */
int seg_fault() {
/* Fork a process so that userinit doesn't take the hit. stack_overflow and */
/* seg_fault will kill the process that executes it. */
  int pid = fork();
  if(NULLPID == pid) {
    int arr[4];
    arr[400] = 1; //Segmentation Fault
    led(LED_GREEN, LED_ON);
  }
  else if(0 == pid) {
    return -1;
  }
  return wait(pid);
}

/**
 * @brief
 *   Test out the yield system call.
 * @post
 *   The blue led will turn off if any system calls return with failure.
 */
void yield_test() {
  pid_t pids[5];
  for(int i = 0; i < 5; i++) {
    if(0 == (pids[i] = fork())) {
        led(LED_BLUE, LED_OFF);
    }
    if(NULLPID == pids[i]) {
      yield();
      exit(EXIT_SUCCESS);
      led(LED_BLUE, LED_OFF);
    }
  }
  for(int i = 0; i < 5; i++) {
    if(-1 == wait(pids[i])) {
      led(LED_BLUE, LED_OFF);
    }
  }
}

/**
 * @brief
 *   Shell main. The first user program run by the kernel after reset.
 */
int smain() {
  stringtest();
  forktest();
  yield_test();
  seg_fault();
  stack_overflow();
	return 0;
}
