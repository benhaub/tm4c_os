/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : kernel_services.c                                               *
 * Synopsis : Supported kernel services for users.                            *
 * Date     : July 18th, 2019                                                 *
 *****************************************************************************/
#include <hw.h>
#include <mem.h> /* in sysexit(), for free_stackpage() */
#include <proc.h>
#include <types.h>
#include <stdarg.h>
#include <cstring.h>
#include <kernel_services.h>

/*
 * IMPORTANT:
 *   Since all the system calls are executed in handler mode, any fault that
 *   is triggered here is an escalation to a hard fault.
 */

/* From proc.c */
extern int maxpid;
extern struct pcb ptable[];

/**
 * Creates a new process. The parent forks the child. parent returns the pid
 * of the new process, child returns NULLPID. Returns -1 on failure.
 */
int sysfork() {
	struct pcb *child = reserveproc(NULL);
	if(NULL == child) {
		return -1;
	}
	struct pcb *parent = currproc();
  parent->numchildren++;
	child->context.pc = parent->context.pc;
/* Number of bytes being used in the parent stack */
  word pstackuse = stacktop(parent->rampg) - parent->context.sp;
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
 */
int syswait(int pid) {
	struct pcb *waiting = currproc();
	waiting->state = WAITING;
	waiting->waitpid = pid;
	return 0;
}

/**
 * Clears out the pcb of the process and notifies it's parent of the exit.
 */
int sysexit(int exitcode) {
	struct pcb *exitproc = currproc();
	exitproc->context.pc = 0;
	exitproc->context.sp = 0;
	exitproc->context.lr = 0;
	exitproc->context.r0 = 0;
	int i;
	if(maxpid == exitproc->pid) {
		for(i = maxpid; i >= 0; i--) {
			if(RUNNABLE == ptable[i].state || RESERVED == ptable[i].state) {
				maxpid = i;
				break;
			}
		}
	}
	free_stackpage(exitproc->rampg);
	exitproc->pid = NULLPID;
/* If this process was the child of another, subtract it's number of children */
	if(exitproc->ppid != NULLPID) {
		pidproc(exitproc->ppid)->numchildren--;
	}
	exitproc->ppid = NULLPID;
  exitproc->waitpid = NULLPID;
  if(0 != exitproc->numchildren) {
    printk("Parent with pid %d exited with children\n\r", exitproc->numchildren);
  }
  exitproc->state = UNUSED;
	exitproc->initflag = 1;
  if(NULL != exitproc->name) {
    strncpy(exitproc->name, "\0", 1);
  }
/* Return the exit code to the parent */
  if(exitcode != 0) {
    return 1;
  }
  return exitcode;
}

/**
 * Print some characters using the uart.
 * @param msg
 *   The characters to print.
 * @return
 *   0 if the full message was printed, -1 if it was not all printed.
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
 * Exactly the same as printf except that a call is made directly to syswrite.
 */
void printk(const char *s, ...) {
  word hex; /* Holds values for hex numbers */
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
  char hex_string[sizeof(word)*2+2];
  char int_string[sizeof(word)*2+2];
  va_list format_strings;
  va_start(format_strings, s);
/* Print one char at a time, inserting the va_args whenever a specifier is */
/* encountered. */
  while(s[i] != '\0') {
    if(s[i] == '%') {
      switch(s[++i]) {
      case('x') :
        hex = va_arg(format_strings, word);
        memset(hex_string, 0, sizeof(word)*2+2);
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
        memset(int_string, 0, sizeof(word)*2+2);
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
        memset(int_string, 0, sizeof(word)*2+2);
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
