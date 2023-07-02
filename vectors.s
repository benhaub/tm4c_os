/******************************************************************************
 *Authour : Ben Haubrich                                                      *
 *File    : vectors.s                                                         *
 *Synopsis: vector table, fault & reset handlers for ARM Cortex M-series      *
 *Date    : May 6th, 2019                                                     *
 *****************************************************************************/
/* Please refer to "Using as - The GNU Assembler" for all syntax. Only the
/* opcodes (ldr, mov, str) and the operands they accept are ARM specific. */
/* Some machine configurations provide additional directives such as  */
/* .thumb and .syntax for ARM. */
  .syntax unified
  .thumb

/* If you change the stack size, make sure to adjust the MAX_PROC define in */
/* mem.h */
  .section .stack
STACK_BOTTOM:
  .skip 0x800, 0x0

/* Constants */	
  .text
/* Kernel Ram usage. */
  .global KRAM_USE
  .set KRAM_USE, Vectors
/* Reset_EXCP needs to be global so it can be used as an entry point. */
  .global Reset_EXCP
/* interrupt_enable is externed in handlers.c for svc calls. */
  .global interrupt_enable
/* A way to dance around and deal with exception mechanisms */
  .global systick_context_save
/* yield calls the systick handler to perform scheduling. */
  .global SYST_EXCP
/* Called by mm_handler when a mstke error occurs */
  .global mstke_repair
/* Called by systick_handler when an a process that has exited yeilds */
  .global switch_to_msp
/* Called by sysexit so that we can call the scheduler instead of yield */
  .global switch_to_privledged
/* Called by systick_handler to save the psp in the pcb */
  .global get_psp
  .global get_msp

  .section .intvecs

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {STACK_BOTTOM, Reset_EXCP,...,SYST_EXCP}; */

/**
 * Note that this is the KERNEL stack pointer not a user stack pointer.
 * They are unrelated. The kernel can have a different sized stack than a
 * user.
 * See Pg. 526 of the datasheet. The presence of this vector table instructs
 * the processor to load the stack pointer and pc from here and start running
 * the user application stored in flash instead of executing the boot loader
 * stored in ROM
 */
Vectors:
  .word STACK_BOTTOM + 0x800
  .word Reset_EXCP      /* Reset Exception */
  .word NMI_EXCP        /* Non-maskable interrupt */
  .word HFAULT          /* Hard Fault */
  .word MM_FAULT        /* Memory Management Fault*/
  .word BFAULT          /* Bus Fault */
  .word UFAULT          /* Usage Fault */
  .word 0               /* Reserved Space */
  .word 0               /* Reserved Space */
  .word 0               /* Reserved Space */
  .word 0               /* Reserved Space */
  .word SVC_EXCP        /* Supervisor Call */
  .word DM_EXCP         /* Debug Monitor */
  .word 0               /* Reserved Space */
  .word PSV_EXCP        /* PendSV */
  .word SYST_EXCP       /* SysTick */
  .word GPIO_PORTA
  .word GPIO_PORTB
  .word GPIO_PORTC
  .word GPIO_PORTD
  .word GPIO_PORTE
  .word UART0
  .word UART1
  .word SSI0

  .text

  .type Reset_EXCP, %function
Reset_EXCP: .fnstart
/* Calculate how much ram the kernel is using so we know where to start */
/* allocating ram pages for user programs. See get_stackspace(). */
/* Where ever the top of stack is determines how much space the kernel is */
/* using. The end of the kernel is the stack top because of how the linker */
/* arranges data storage in SRAM. */
  mov r0, #0x400 /* Stack Size */
  mov r1, sp
  sub r1, r1, #0x20000000
/* Divide this by the current position of the sp to get an integer for */
/* get_stackspace. */
  udiv r1, r0
/* Push the value onto the first spot in the stack marked by the symbol */
/* KRAM_USAGE. */
  push {r1}
/* Enable the FPU. Taken from tivaware bootloader code, but also detailed */
/* on page 74 of the Cortext-M4 TRM. It is enabling bits within the register */
/* located at 0xE000ED88. */
  movw    r0, #0xED88
  movt    r0, #0xE000
  ldr     r1, [r0]
  orr     r1, r1, #0x00F00000
  str     r1, [r0]
  b init
  .fnend

  .type NMI_EXCP, %function
NMI_EXCP: .fnstart
  b nmi_handler
  .fnend

  .type HFAULT, %function
HFAULT: .fnstart
  b hfault_handler
  .fnend

/* The faulting process will exit() when the bus fault handler is done via the*/
/* Exception return mechanism. It is not possible for the kernel to cause an */
/* mm fault because it has access to the entire memory map. */
	.type MM_FAULT, %function
MM_FAULT: .fnstart
  mrs r1, psp
/* If the bus fault was caused by the kernel, do not exit, just loop in the */
/* fault handler. The value being compared is EXEC_RETURN on pg.111 in the */
/* datasheet. */
  cmp lr, #0xFFFFFFED
  beq MUser
  mov r0, #1 //Set args for mm_handler
MUser:
  mov r0, #0 //Set args for mm_handler
/* Check the fault stat register for stacking errors. If this happens we need */
/* to return to privledged mode with msp to recover. */
  movw r4, #0xED28
  movt r4, #0xE000
  ldr r4, [r4]
  ands r4, r4, #0x10
  bne MSTKE
/* Replace the exception stack frame PC with exit so we can kill this process */
  ldr r4,=exit
  str r4, [r1, #24]
/* Get the Pre-IRQ top of stack for the mm_handler to examine. */
  add r1, #108
  b mm_handler
MSTKE:
/* Save the stacked PC before modifying it. This stack doesn't belong to us */
/* so we have to leave it the way we found it. */
  ldr r4, [r1, #24] //r4's value must be maintained for mstke_repair
  ldr r3,=mstke_exit
/* Replace the exception stack frame PC with exit so we can kill this process */
  str r3, [r1, #24]
/* Change thread mode privledge level to privledged so we can switch stacks
/* in mstke_repair to msp. When we return from this handler, we are still */
/* using psp to pop off the exception stack, but at the end of the exception */
/* return sequence we will be using msp and be privledged. */
  mrs r3, CONTROL
  bic r3, r3, #0x1
  msr CONTROL, r3
/* Clear out the stacked PSR (except for the thumb bit) so that we can return */
/* properly. Since there was a stacking error, the PSR could not be pushed and*/
/* likely contains invalid values. We don't know it's original state, but it */
/* doesn't matter. This process has blown it's stack and will exit. We only */
/* need it to survive long enough so that it can exit. */
/* See the integrity checks on EXC_RETURN in the Armv7-m technical */
/* reference manual. */
  movw r3, #0x0000
  movt r3, #0x0100
  ldr r5, [r1, #28]
  str r3, [r1, #28]
  add r1, #108
  b mm_handler
  .fnend

/*
 * Repairs the stack that we overflowed on by replacing the values of the pc
 * and xPSR with what they were before.
 *
 * Remember that the exception return stack spilled over into another stack, so
 * we are not restoring actual stacked PC values and PSR values, but rather
 * random stacked data from another process that happens to be contained in the
 * spot where the exception return stack normally stores the PC and PSR.
 */
  .type mstke_repair, %function
mstke_repair: .fnstart
  str r4, [r0, #24] //Restore the PC
  str r5, [r0, #28] //Restore the PSR
  bx lr
  .fnend
/*
 * exit the process that caused the stacking error.
 */
.type mstke_exit, %function
mstke_exit: .fnstart
  bl switch_to_msp
  bl sysexit
  b scheduler
 .fnend

  .type BFAULT, %function
BFAULT: .fnstart
/* The faulting process will exit() when the bus fault handler is done via the*/
/* Exception return mechanism. */
//TODO: This part doesn't need to be done if the stack is msp
  mrs r1, psp
  ldr r0,=exit
  str r0, [r1, #24]
/* If the bus fault was caused by the kernel, do not exit, just loop in the */
/* fault handler. The value being compared is EXEC_RETURN on pg.111 in the */
/* datasheet. */
  cmp lr, #0xFFFFFFED
  beq BUser
  mov r0, #1
  b b_handler
BUser:
  mov r0, #0
  b b_handler
  .fnend

  .type UFAULT, %function
UFAULT:	.fnstart
/* The faulting process will exit() when the bus fault handler is done via the*/
/* exception return mechanism. */
  mrs r1, psp
  ldr r0,=exit
  str r0, [r1, #24]
  cmp lr, #0xFFFFFFED
  beq UUser
  mov r0, #1
  b u_handler
UUser:
  mov r0, #0
  b u_handler
  .fnend

  .type SVC_EXCP, %function
SVC_EXCP: .fnstart
/* Use the svc immediate value to determine if this is syscall or a syscalln */
  mrs r9, psp
/* load the PC from the exception stack */
  ldr r9, [r9, #24]
/* The exception stack saves the address of the next instruction, not the */
/* instruction that caused the exception, so we have to go back to it. */
  ldr r9, [r9, #-2]
  and r9, r9, #0xFF
  cmp r9, #0
  beq Syscall0
   b svc_handler
Syscall0:
/* Put the link register in the pcb */
  str r2, [r1, #4]
/* Restore stack pointer to where it was before system call. */
  mrs r9, psp
/* 108 is the size of the floating point exception frame as well as a stack */
/* push of 8 bytes from the prologue of the syscalln functions. */
  add r9, #116
/* Put the stack pointer in the pcb */
  str r9, [r1]
  b svc_handler
  .fnend

  .type DM_EXCP, %function
DM_EXCP: .fnstart
  b dm_handler
  .fnend

  .type PSV_EXCP, %function
PSV_EXCP: .fnstart
  b psv_handler
  .fnend

/*
 * Saves some of the process context before branching to the systick handler
 * to execute the desired kernel service. Control is returned in privledged
 * mode so that the rest of the context can be saved properly using
 * systick_context_save()
 *
 * The reason we don't do a direct branch to the handler is to avoid context
 * switching while in handler mode. The processor's exception mechanism makes
 * switching in handler mode very difficult. The exception mechanism exits
 * handler mode on the bx lr, and stack saving happens in systick_context_save. The
 * purpose of this code here is too save r0, pc and lr because it will be
 * changed when we enter the exeption handler c code. systick_context_save will need
 * these values in order to save the context correclty.
 */
  .type SYST_EXCP, %function
SYST_EXCP: .fnstart
/* Save the context to msp stack */
  mrs r9, psp
  mov r10, r14
  ldr r11, [r9, #24] //Exception stack PC
  ldr r14, [r9, #20] //Exception stack LR
  /* Make sure the thumb bit is set for the pc instruction. */
  ands r9, r11, #0x1
  bne Thumb
  add r11, r11, #0x1
Thumb:
  push {r0-r8, r11, r12, r14}
  mov r14, r10

  mrs r0, psp
/* Save exception stack value for the PC so that we can get back to our */
/* execution path in swtch() in context.s */
  ldr r1, [r0, #24]
/* Place syst_handler on the stacked pc. Exception mechanism retores it to lr*/
  ldr r2,=syst_handler
  str r2, [r0, #24]
/* Abandon any load or store multiple instructions and continue them from the */
/* beginning when this process returns to it's context. */
/* See ARMv7-M Architecture Reference Manual Pg. 543 - Sect. B1.5.10 */
  ldr r2, [r0, #28]
  bic r2, r2, #0xF000
  str r2, [r0, #28]
/* Change thread mode privledge level to privledged so that we can switch */
/* stacks */
  mrs r9, CONTROL
  bic r9, r9, #0x1
  msr CONTROL, r9
  ISB
/* Exception return mechanism will return r0-r3, r12, lr, pc and xPSR */
/* to pre-exception values. */
  bx lr
  .fnend

/**
 * Save the process context after a systick interrupt. It is executed from
 * thread mode so that it is possible to save stacks and context switch
 * properly. It makes sure the process stack is returned to it's orginal state,
 * and then pushes the context onto the stack and saves the stack pointer (psp)
 * before switching stacks from psp to msp. The corresponding pop is made from
 * swtch so make sure that the push here and pop in swtch() are consistent.
 * @pre
 *   You must be in a privledged processor state to execute this function
 *   otherwise a usage fault will occur.
 */
  .type systick_context_save, %function
systick_context_save: .fnstart
  mov r9, r14
  pop {r0-r8, r11, r12, r14}

  mrs r10, CONTROL
  orr r10, r10, #0x2
  msr CONTROL, r10

/* Changing what registers we save on the stack is not as easy as just adding */
/* them here. The corresponding pop in swtch() must also be changed and the */
/* size of the stack must be edited by changing the value of CTXSTACK in */
/* mem.h */
  push {r0-r8, r11, r12, r14}
  mov r14, r9

  bx lr
  .fnend

.type GPIO_PORTA, %function
GPIO_PORTA: .fnstart
  b gpio_porta_handler
  .fnend
.type GPIO_PORTB, %function
GPIO_PORTB: .fnstart
  b gpio_portb_handler
  .fnend
.type GPIO_PORTC, %function
GPIO_PORTC: .fnstart
  b gpio_portc_handler
  .fnend
.type GPIO_PORTD, %function
GPIO_PORTD: .fnstart
  b gpio_portd_handler
  .fnend
.type GPIO_PORTE, %function
GPIO_PORTE: .fnstart
  b gpio_porte_handler
  .fnend
.type UART0, %function
UART0: .fnstart
  b uart0_handler
  .fnend
.type UART1, %function
UART1: .fnstart
  b uart1_handler
  .fnend
.type SSI0, %function
SSI0: .fnstart
  b ssi0_handler
  .fnend

/**
 * Change the processor state to either enable or disable interrupts.
 * use 1 as a parameter to enable, 0 to disable.
 * interrupt_enable(int enable).
 * @pre
 *   You must be in a privledged processor state to execute this function
 *   otherwise a usage fault will occur.
 */
	.type interrupt_enable, %function
interrupt_enable: .fnstart
  cmp r0, #0
  beq Disable
  cpsie i
  b Return
Disable:
  cpsid i
Return:
  bx lr
.fnend

.type switch_to_msp, %function
switch_to_msp: .fnstart
  mrs r3, CONTROL
  bic r3, r3, #0x2
  msr CONTROL, r3
  ISB
  bx lr
.fnend

.type switch_to_psp, %function
switch_to_psp: .fnstart
  mrs r3, CONTROL
  orr r3, r3, #0x2
  msr CONTROL, r3
  ISB
  bx lr
.fnend

.type switch_to_privledged, %function
switch_to_privledged: .fnstart
  mrs r3, CONTROL
  bic r3, r3, #0x1
  msr CONTROL, r3
  ISB
  bx lr
.fnend

.type switch_to_unprivledged, %function
switch_to_unprivledged: .fnstart
  mrs r3, CONTROL
  orr r3, r3, #0x1
  msr CONTROL, r3
  ISB
  bx lr
.fnend

.type get_psp, %function
get_psp: .fnstart
  mrs r0, psp
  bx lr
.fnend

.type get_msp, %function
get_msp: .fnstart
  mrs r0, msp
  bx lr
.fnend

.end
