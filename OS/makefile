###############################################################################
#Authour	:	Ben Haubrich
#File			: makefile
#Synopsis	:	makefile for tm4c_os
###############################################################################

#Search for header files here as well as the current working directory
VPATH=include

#Define the C compiler
CC=arm-none-eabi-gcc
#Architecture specific options for C compiler
CPU=-mcpu=cortex-m4 \
		-mtune=cortex-m4
FPU=-mfpu=fpv4-sp-d16 \
		-mfloat-abi=hard
PART=TM4C123GH6PM
#Flags for C compiler.
CLFAGS=-specs=rdimon.specs \
			 -mthumb \
			 ${CPU} \
			 ${FPU} \
			 -ffunction-sections \
			 -fdata-sections \
			 -std=c89 \
			 -DM \
			 -Wall \
			 -pedantic \
			 -DPART_${PART}
#Optionally include the symbol table for debugging.
ifdef DEBUG
	CFLAGS+=-g
endif

#Assembler flags.
AFLAGS=-mthumb \
			 ${CPU} \
			 ${FPU} \
			 -MD

#Flags for the linker. Not invoking the linker directly fixed a lot of problems
#with library linking and functions not being found. Using -specs=nano.specs or
#nosys.specs or rdimon.specs stops an error about _exit() not being found.
LDFLAGS=-Wl,--gc-sections \
			  -Wl,-Map tm4c_os.map \
				-Wl,-Tlink.lds

#Define objcopy to extract out elf headers from binaries. Bare metal code does
#not have the ability to read these properly and will try to execute them which
#will like cause undefined instruction errors.
OBJCOPY=arm-none-eabi-objcopy
OBJCFLAGS=-O binary

tm4c_os.bin: tm4c_os.elf
	${OBJCOPY} ${OBJCFLAGS} tm4c_os.elf tm4c_os.bin

tm4c_os.elf: vectors.o init.o
	${CC} ${CFLAGS} ${LDFLAGS} -otm4c_os.elf vectors.S init.o

init.o: init.c
	${CC} ${CFLAGS} -c -Dgcc -Iinclude init.c

vectors.o: vectors.S
	${CC} ${CFLAGS} ${AFLAGS} -c -Dgcc vectors.S

clean:
	rm -rf *.o *.d tm4c_os.elf tm4c_os.bin
