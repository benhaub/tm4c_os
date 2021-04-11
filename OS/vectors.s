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
STACK_TOP:
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
/* yeild calls the systick handler to perform scheduling. */
  .global SYST_EXCP

	.section .intvecs

/* Pg.103, datasheet - Table 2-8 details the vector table. */
/* In C, this is similar to: */
/* unsigned int Vectors[16] = {STACK_TOP, Reset_EXCP,...,SYST_EXCP}; */

/**
 * Note that this is the KERNEL stack pointer not a user stack pointer.
 * They are unrelated. The kernel can have a different sized stack than a
 * user.
 */
Vectors:
	.word STACK_TOP + 0x800 /* Boot loader gets kernel stack pointer from here */
	.word Reset_EXCP	/* Reset Exception */
	.word NMI_EXCP		/* Non-maskable interrupt */
	.word HFAULT			/* Hard Fault */
	.word MM_FAULT		/* Memory Management Fault*/
	.word BFAULT			/* Bus Fault */
	.word UFAULT			/* Usage Fault */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word 0						/* Reserved Space */
	.word SVC_EXCP		/* Supervisor Call */
	.word DM_EXCP			/* Debug Monitor */
	.word 0						/* Reserved Space */
	.word PSV_EXCP		/* PendSV */
	.word SYST_EXCP		/* SysTick */

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
						b main
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
/* Check the fault stat register for stacking errors. If this happens we need */
/* to return to privledged mode with msp to recover. */
          movw r0, #0xED28
          movt r0, #0xE000
          ldr r0, [r0]
          ands r0, r0, #0x10
          bne MSTKE
/* Replace the exception stack frame PC with exit so we can kill this process */
          ldr r0,=exit
          str r0, [r1, #24]
/* Get the Pre-IRQ top of stack for the mm_handler to examine. */
          add r1, #108
          mov r0, #0
          b mm_handler
MSTKE:
/* Replace the exception stack frame PC with mstke_reapair so we can repair */
/* the stack and then kill the process. */
          ldr r0,=mstke_repair
/* Save the stack value before modifying it. This stack doesn't belong to us */
/* so we have to leave it the way we found it. */
          ldr r4, [r1, #24]
          str r0, [r1, #24]
/* Clear out the stacked PSR (except for the thumb bit) so that we can return */
/* properly. Since there was a stacking error, the PSR could not be pushed */
/* likely contains invalid values. We don't know it's original state so we do */
/* Consult the integrity checks on EXC_RETURN in the Armv7-m technical */
/*reference manual if this does not make sense. */
          movw r0, #0x0000
          movt r0, #0x0100
          ldr r5, [r1, #28]
          str r0, [r1, #28]
/* Get the Pre-IRQ top of stack for the mm_handler to examine. */
          add r1, #108
/* Change thread mode privledge level to privledged and switch stacks. When */
/* we return from this handler, we are still using psp to pop off the */
/* exception stack, but at the end of the exception return sequence we will */
/* be using msp and be privledged. */
					mrs r3, CONTROL
					bic r3, r3, #0x3
					msr CONTROL, r3
/* ISB flushes the processor pipeline so that all instructions that follow */
/* are fetched from cache or memory. This ensures that we will start using */
/* the new stack. */
          ISB
          b mm_handler
					.fnend

/*
 * Repairs the stack that we overflowed on by replacing the values of the pc
 * and IPSR with what they were before. The function then exits the process
 * that caused the stack overflow.
 */
	.type mstke_repair, %function
mstke_repair: .fnstart
              str r4, [r1, #24]
              str r5, [r1, #28]
              b exit
				      .fnend

	.type BFAULT, %function
BFAULT: .fnstart
/* The faulting process will exit() when the bus fault handler is done via the*/
/* Exception return mechanism. */
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
         str r2, [r1, #4]
/* Restore stack pointer to where it was before system call. */
         mrs r9, psp
/* 108 is the size of the floating point exception frame as well as a stack */
/* push of 8 bytes from the prologue of the syscalln functions. */
         add r9, #116
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
 *
 * @note
 *  Do not use r10 here. If the syscallasm code is interrupted by systick then
 *  using r10 will overwrite the pcb that is used to get the return value from
 *  the kernel services.
 * @sa sysc
 */
	.type SYST_EXCP, %function
SYST_EXCP: .fnstart
/* Get the processes stack pointer and save it */
					mrs r0, psp
/* Save the value of the exception stack pc. */
					ldr r5, [r0, #24]
/* Save the value of the exception stack lr. */
					ldr r6, [r0, #20]
/* Make sure r5 has bit one set for thumb instructions. */
					ands r8, r5, #0x1
					bne Thumb
					add r5, r5, #0x1
Thumb:
					ldr r3,=syst_handler
/* Place syst_handler on the stacked pc. Exception mechanism retores it to lr*/
					str r3, [r0, #24]
/* Save r7 in case syst_handler changes it. */
					mov r8, r7
/* Exception return mechanism will return r0-r3 to pre-exception values. */
/* r4, r5, and r6 remain unscathed. */
/* Change thread mode privledge level to privledged so that we can switch */
/* stacks in systick_context_save(). */
					mrs r3, CONTROL
					bic r3, r3, #0x1
					msr CONTROL, r3
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
/* Transfer r0 to r9 so that r0 can be returned to it's saved pre-systick */
/* interrupt value. */
							        mov r9, r0
/* Save the lr before moving the saved pc from systick isr into it. */
							        mov r4, r14
							        mov r14, r6
/* Changing what registers we save on the stack is not as easy as just adding */
/* them here. The corresponding pop in swtch() must also be changed and the */
/* size of the stack must be edited by changing the value of CTXSTACK in */
/* mem.h */
							        push {r0-r3, r5, r7, r10, r12, r14}
/* Save the stack pointer to context struct */
							        str sp, [r9] 
/* Restore lr to it's original value */
							        mov r14, r4
/* Switch stacks to MSP. */
							        mrs r3, CONTROL
							        bic r3, r3, #0x2
							        msr CONTROL, r3
                      ISB
							        bx lr
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
Disable:				  cpsid i
Return:					  bx lr
								  .fnend
	.end
