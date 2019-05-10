###############################################################################
#Authour	:	Ben Haubrich
#File			: makefile
#Synopsis	:	makefile for tm4c_os
###############################################################################

#Define the C compiler
CC=arm-none-eabi-gcc
#Architecture specific options for C compiler
CPU=-mcpu=cortex-m4 -mtune=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=hard
#Flags for C compiler.
CLFAGS=-mthumb ${CPU} ${FPU} -ffunction-sections -fdata-sections -MD -std=c99 \
			 -Wall -pedantic -DPART_${PART} -c
#Optionally include the symbol table for debugging.
ifdef DEBUG
	CFLAGS+=-g
endif

#Assembler flags.
AFLAGS=-mthumb ${CPU} ${FPU} -MD

#Define the linker
LD=arm-none-eabi-ld
#Flags for the linker. There are no entry points defined because the bootloader
#already knows where it wants to find the OS.
LDFLAGS=--gc-sections

#Define objcopy to extract out elf headers from binaries
OBJCOPY=arm-none-eabi-objcopy
OBJCFLAGS=-O binary

#Once I have c files the target, recipie and rule can go here

tm4c_os.o: vectors.S
	${CC} ${AFLAGS} -Dgcc -c vectors.S

tm4c_os.elf: vectors.o
	${LD} ${LDLAGS} -o tm4c_os.elf -Map tm4c_os.map

tm4c_os.bin: tm4c_os.elf
	${OBJCOPY} ${OBJCFLAGS} tm4c_os.elf

clean:
	rm -rf *.o tm4c_os.elf tm4c_os.bin
