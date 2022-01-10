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
  //for(int i = 0; i < 10E7; i++);
  return;
}

/**
 * @brief
 *   The led should be purple when this test is done.
 *   First the parent turns on the green led, then forks NPROC+10 processes.
 *   10 of these processes should fail to fork because there are no more unused
 *   processes in the process table. The children all turn off the green led and
 *   then exit while the parent turn on the blue led and waits for them. When
 *   all the children have exited, the parent turns on the red led and then
 *   exits.
 */
void forktest() {
	led(LED_GREEN, LED_ON);
	int i;
  //pid_t pids[NPROC+10];
  pid_t pids[1];
	//for(i = 0; i < NPROC+10; i++) {
	for(i = 0; i < 1; i++) {
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
	//for(i = 0; i < NPROC+10; i++) {
	for(i = 0; i < 1; i++) {
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
 */
void stack_overflow_child() {
/* Allocate an array that is greater than STACK_SIZE */
  int big_array[384]; //1.5KB
/* Call wait to force a write to the stack. The array declaration only moves */
/* the stack pointer. Nothing has been written to cause a memory fault yet. */
  wait(NULLPID);
  led(LED_GREEN, LED_ON);
  exit(EXIT_SUCCESS);
}

int stack_overflow() {
  int pid = fork();
  if(NULLPID == pid) {
    stack_overflow_child();
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
 *   Test out the stack repairing abilities of the OS.
 *
 * This function will create two process, and one process will create an array
 * of the size of a user stack and then make a system call. The system call will
 * write to the stack and overflow into the next stack in use the by second
 * process. This second process will have a few variables and increment them.
 *
 * The second process will check after the increment that the value is greater
 * than the previous value by the amount of the increment. If it's not then the
 * red LED will turn off.
 */
void stack_repair_child() {
  //Child process
  char arr[STACK_SIZE];
  wait(NULLPID); //SVC exception return stack will cause a mem fault.
  exit(EXIT_SUCCESS);
}

void stack_repair_parent() {
  int i = 0;
  int j = 0;
  int k = 0;
  
  led(LED_RED, LED_ON);

  //Parent process
  i += 5; j += 6; k += 7;
  yield(); //Yield so that the child process runs before we check.
  if(i != 5 || j != 6 || k != 7) {
    led(LED_RED, LED_OFF);
    exit(EXIT_SUCCESS); //Exit and return to the scheduler
  }
}

void stack_repair() {
  pid_t ret[2];
  int i;
  ret[0] = fork();
  if(NULLPID == ret[0]) {
    stack_repair_parent();
    exit(EXIT_SUCCESS);
  }
  ret[1] = fork();
  if(NULLPID == ret[1]) {
    stack_repair_child();
    exit(EXIT_SUCCESS);
  }
  for(i = 0; i < 2; i++) {
    wait(ret[i]);
  }
}

/**
 * @brief
 *   Shell main. The first user program run by the kernel after reset.
 */
int smain() {
  if(stringtest())
    return 0;
  forktest();
//  yield_test();
//  seg_fault();
//  stack_overflow();
//  stack_repair();
  exit(EXIT_SUCCESS);
}
