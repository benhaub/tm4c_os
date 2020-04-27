/******************************************************************************
 *Authour : Ben Haubrich                                                      *
 *File    : context.s                                                         *
 *Synopsis: Context switching and processor privledge control                 *
 *Date    : June 25th, 2019                                                   *
 *****************************************************************************/
	
  .syntax unified
  .thumb

/*
 * Context switcher. Load the registers from the process that is running.
 * Registers from the previous process are saved at the systick interrupt
 * before the cpu is switched to privledged and the stack pointer is switched
 * to msp.
 * void swtch(word sp), where sp is the top of the stack of the process to
 * switch to.
 */
  .global swtch
  .type swtch, %function
swtch: .fnstart
        msr psp, r0 //Switch psp stack to new one
/* Switch to psp and unprivledge mode. */
        mrs r2, CONTROL
        orr r2, r2, #0x3
        msr CONTROL, r2
        pop {r0-r3, r5, r7, r12, r14}
        bx r5
       .fnend

/*
 * All new processes run initcode first to set up cpu registers to make it
 * look as if the process had been interrupted by an svc with it's registers
 * pushed on the stack. initcode places the value of pc into the r5 register.
 * initcode(word sp), where sp is the top of the stack of the new process.
 */
  .global initcode
  .type initcode, %function
initcode: .fnstart
          add r1, r0, #4
          ldr r2, [r1] //context.pc
          ldr r3, [r0] //context.sp
//Move pc to r5's spot on the stack so it gets branched to in swtch()
          str r2, [r3, #16]
          add r1, r0, #12
          ldr r2, [r1] //context.r0
//store context.r0 on the stack in r0's spot
          str r2, [r3]
//Move stack pointer to r2
          mov r2, r3
//store context.sp on the stack in r7's spot
          str r2, [r3, #20]
          bx lr
          .fnend	
	.end
	
