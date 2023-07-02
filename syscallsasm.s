/******************************************************************************
 * Authour  : Ben Haubrich                                                    *
 * File     : syscalls.c                                                      *
 * Synopsis : Context switcher just for system calls. Always switches to kernel
 *            context from user context.                                      *
 * Date     : July 18th, 2019                                                 *
 *****************************************************************************/
  .syntax unified
  .thumb

/**
 * Almost the same as the context switch. Only difference is that we don't 
 * need to update the stack pointer, and we switch to msp (instead of psp)
 * and to privledged (instead of unprivledged). Privledge levels are handled
 * automatically by the exception entry and return mechanisms (which are
 * triggered by the svc instruction).
 *
 * This code is interruptable by clock interrupts. Any register that is not
 * pushed onto the stack as part of the context switch will be lost.
 */

/**
 * syscall is unique from other syscalls in that it is the syscall routine that
 * services calls to fork. It is the only syscall asm routine that makes writes
 * into the ptable for storing the program counter and stack pointer. This must
 * be done by privledged software since the MPU restricts user process writes to
 * only the memory region in which their stack resides. Consequently, some of
 * this routine is serviced in the svc_handler instead of here (instructions
 * that make writes to the ptable).
 */
  .global syscall
  .type syscall, %function
syscall: .fnstart
/* Put the program counter and all general purpose registers into the */
/* processes struct context. This is required for fork(). The pc from the */
/* original system call is in the stacked link register. The corresponding */
/* str is done in the svc handler. */
  ldr r2, [sp, #4]
/* The immediate value for svc is not used except for this assembly routine */
/* because fork requires the stack to be saved in the pcb context. */
/* The number used for determining the kernel service is passed through as an */
/* argument to syscalln() (here that manifests itself as r0). The cortex-m4 */
/* exception entry and return model take care of context. */
  svc #0
/* Move the returned value from the syscall to r0 so that the kernel */
/* services return the right value */
  ldr r0, [r1, #12]
  bx lr
  .fnend

/**
 * The assembly routine for system calls that have one argument.
 */
  .global syscall1
  .type syscall1, %function
syscall1: .fnstart
/* Argument needs to be placed in r1 so svc_handler gets it in arg1. First */
/* save currproc. */
  push {r1}
/* Move the syscalls argument over to r1 and overwrite the currproc pointer. */
  mov r1, r2
/* The immediate value for svc is not used, however, make sure that a non-zero
/* value is used since syscall needs the immediate value on svc instructions. */
/* The number used for determining the kernel service is passed through as an */
/* argument to syscall() (here that manifests itself as r0). */
  svc #1
/* Move the returned value from the syscall to r0 so that the kernel */
/* services return the right value */
  pop {r1}
  ldr r0, [r1, #12]
  bx lr
  .fnend

/**
 * The assembly routine for system calls that have two arguments.
 */
  .global syscall2
  .type syscall2, %function
syscall2: .fnstart
  push {r1}
/* Shift all the arguments over so it lines up with arguments in the handler */
  mov r1, r2
  mov r2, r3
/* The svc immediate value for any routine except syscall must be non-zero. */
  svc #2
  pop {r1}
  ldr r0, [r1, #12]
  bx lr
  .fnend

  .global syscall3
  .type syscall3, %function
syscall3: .fnstart
/* Save r1 to r8 */
  mov r8, r1
  mov r1, r2
  mov r2, r3
/*// Arguments after the 4th are pushed onto the stack. */
  pop {r3}
/* Push r8 onto the stack to save the currproc pointer .*/
  push {r8}
  svc #3
  pop {r1}
/* load the returned value in currproc to r0. See syscreturn in handlers.c */
  ldr r0, [r1, #12]
  bx lr
  .fnend

  .end
