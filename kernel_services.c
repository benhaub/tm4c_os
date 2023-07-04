/**************************************************************************//**
 * @author  Ben Haubrich                                                    
 * @file    kernel_services.c                                              
 * @date    July 18th, 2019                                               
 * @details \b Synopsis: \n Supported kernel services for users.                          
 *****************************************************************************/
#include "hw.h"
#include "mem.h" /* in sysexit(), for free_stackpage() */
#include "proc.h"
#include "stdarg.h"
#include "cstring.h"
#include "kernel_services.h"

/*
 * IMPORTANT:
 *   Since all the system calls are executed in handler mode, any fault that
 *   is triggered here is an escalation to a hard fault.
 */

//! @cond Doxygen_Suppress_Warning
/* From proc.c */
extern int maxpid;
extern struct pcb ptable[];
/* From vectors.s */
extern void SYST_EXCP(); //For yield.
extern void switch_to_privledged(); //For exit
//! @endcond

/**
 * @sa fork
 */
pid_t sysfork() {
	struct pcb *child = reserveproc(NULL);
	if(NULL == child) {
		return 0;
	}
	struct pcb *parent = currproc();
  parent->numchildren++;
	child->context.pc = parent->context.pc;
/* Number of bytes being used in the parent stack */
  uint32_t pstackuse = stacktop(parent->rampg) - parent->context.sp;
/* If the kernel forked this process, then the stack usage may be larger than */
/* the process stack size since the kernel stack is separate from the user */
/* stack. We need to limit the amount of stack copied to 1KB. */
  if(pstackuse > STACK_SIZE) {
    pstackuse = STACK_SIZE;
  }
/* Copy the parent's stack */
  memcpy(
      (void *)(stacktop(child->rampg) - pstackuse),
      (void *)(stacktop(parent->rampg) - pstackuse),
      pstackuse 
  );
/* Adjust the stack pointer of the child to the same offset as the parent. */
  child->context.sp -= pstackuse;
	child->ppid = parent->pid;
/* Child will return NULLPID to the user process. */
	child->context.r0 = NULLPID;
	return child->pid;
}

/**
 * The calling process waits for the process belonging to pid to exit (become
 * un-used). The scheduler handles the stopping and starting of waiting
 * processes.
 * @sa wait
 */
int syswait(int pid) {
	struct pcb *waiting = currproc();
	waiting->state = WAITING;
	waiting->waitpid = pid;
	return 0;
}

/**
 * Clears out the pcb of the process and notifies it's parent of the exit.
 * @sa exit
 */
void sysexit(int exitcode) {
	struct pcb *exitproc = currproc();
	exitproc->context.pc = 0;
	exitproc->context.sp = 0;
	exitproc->context.lr = 0;
	exitproc->context.r0 = 0;
  if(exitproc->pid > MAXPROC || UNUSED == exitproc->state) {
/* Exiting an invalid process is always a failure */
    while(1);
  }
	free_stackpage(exitproc->rampg);
/* If this process was the child of another, subtract it's number of children */
	if(exitproc->ppid != NULLPID) {
		pidproc(exitproc->ppid)->numchildren--;
	}
	exitproc->ppid = NULLPID;
  exitproc->waitpid = NULLPID;
  if(0 != exitproc->numchildren) {
    printk("Parent with pid %d exited with children\n\r", exitproc->pid);
  }
  exitproc->state = UNUSED;
	exitproc->initflag = 1;
  if(NULL != exitproc->name) {
    strncpy(exitproc->name, "\0", 1);
  }

/* Switch to privledged mode so that we can call the scheduler directly when */
/* we return from the svc_handler back to exit() */
  printk("Process with pid %x exited.\n\r", exitproc->pid);
	exitproc->pid = NULLPID;
  switch_to_privledged();
}

/**
 * @brief
 *   Print some characters using the uart.
 * @param msg
 *   The characters to print.
 * @return
 *   0 if the full message was printed, -1 if it was not all printed.
 * @sa write uart1_tchar
 */
int syswrite(const char *msg) {
  int i = 0;
  while(msg[i] != '\0') {
    if(-1 == uart1_tchar(msg[i])) {
      return -1;
    }
    else {
      i++;
    }
  }
  return 0;
}

/**
 * @brief
 *   Print a formatted string to the console
 *
 * Exactly the same as printf except that a call is made directly to syswrite.
 * @param s
 *   The string to print
 * @sa printf write
 */
void printk(const char *s, ...) {
  uint32_t hex; /* Holds values for hex numbers */
  int integer;
  int bytes_remaining = 64; /* Bytes left in the buffer */
  int i = 0;
  int len; /* for holding return value from strlen() */
/* Build strings 64 bytes at a time before sending to the uart to reduce the */
/* amount of system calls to syswrite(). */
  char buf[64] = {'\0'};
/* For concatenating single chars of s */
  char s_char;
/* Strings for holding the string number. Sizes of the arrays are the max */
/* number of characters needed to represent the largest integer on this */
/* processor. hex has 2 extra for "0x" at the beginning. */
  char hex_string[sizeof(uint32_t)*2+2];
  char int_string[sizeof(uint32_t)*2+2];
  va_list format_strings;
  va_start(format_strings, s);
/* Print one char at a time, inserting the va_args whenever a specifier is */
/* encountered. */
  while(s[i] != '\0') {
    if(s[i] == '%') {
      switch(s[++i]) {
      case('x') :
        hex = va_arg(format_strings, uint32_t);
        memset(hex_string, 0, sizeof(uint32_t)*2+2);
        htoa(hex, hex_string);
        len = strlen(hex_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, hex_string, len);
          bytes_remaining -= len;
        }
        else {
          syswrite(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      case('i') :
        integer = va_arg(format_strings, int);
        memset(int_string, 0, sizeof(uint32_t)*2+2);
        itoa(integer, int_string);
        len = strlen(int_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, int_string, len);
          bytes_remaining -= len;
        }
        else {
          syswrite(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      case('d') : /* Same thing as %i */
        integer = va_arg(format_strings, int);
        memset(int_string, 0, sizeof(uint32_t)*2+2);
        itoa(integer, int_string);
        len = strlen(int_string);
        if(bytes_remaining - len > 0) {
          strncat(buf, int_string, len);
          bytes_remaining -= len;
        }
        else {
          syswrite(buf);
          bytes_remaining = 64;
          buf[0] = '\0';
        }
      break;
      }
    }
    else if(bytes_remaining > 0) {
      s_char = s[i];
      strncat(buf, &s_char, 1);
      bytes_remaining--;
    }
    else {
      syswrite(buf);
      bytes_remaining = 64;
      buf[0] = '\0';
    }
    i++;
  }/* Delimits while(s[i] != '\0')*/
  va_end(format_strings);
  syswrite(buf);
  return;
}

/**
 * Set led to the given state with the given colour.
 * @param colour
 *   The colour to turn the led to
 * @param state
 *   Either on or off
 * @return 0 on success, -1 otherwise.
 * @sa led
 */
int sysled(int colour, int state) {
  switch (colour) {
    case 0: state ? led_ron() : led_roff();
            break;
    case 1: state ? led_gron() : led_groff();
            break;
    case 2: state ? led_blon() : led_bloff();
            break;
    default: return -1;
  }
  return 0;
}

/**
 * @sa yield
 *
 * Since yield is called with syscall, we have an updated pcb with the processes
 * stack pointer. We can use this stack pointer to edit the pc of the exception
 * stack so that this process returns to the scheduler instead of the code it
 * was running.
 */
void sysyield() {
  SYST_EXCP();
}

/**
 * @sa spi
 */
void sysspi(int direction, uint8_t *data) {
  if (NULL == data)
    return;

  if (direction) {
    *data = ssi0_receive();
  }
  else {
    ssi0_transmit(*data);
  }
}

/**
 * @sa gpio
 */
void sysgpio(int port, int pin, int state) {
  gpioWrite(port, pin, state);
}

/**
 * @sa delay
 * @pre
 *   Call gptmTimerInit() before calling this function or a bus fault will occur
 */
void sysdelay(uint32_t delayMs) {
  gptmTimerStart(delayMs);
  //TODO: It would be nice if this could yeild or let another process do
  //something while we wait for the timer. We are in interrupt context here so
  //this blocks the entire OS. We might need to make all the syscalln save
  //the context to memory instead of just syscall
  while (gptmWaitForTimeout());
}
