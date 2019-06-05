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
CPU=-mcpu=cortex-m4 -march=armv7e-m -mtune=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=hard
PART=TM4C123GH6PM
#Flags for C compiler.
CFLAGS=-mthumb \
			 -nodefaultlibs \
			 -static \
			 ${CPU} \
			 ${FPU} \
			 -Iinclude \
			 -MD \
			 -Dgcc \
			 -ffunction-sections \
			 -fdata-sections \
			 -std=c99 \
			 -Wall \
			 -pedantic \
			 -DPART_${PART}
#-static: Stops ld from looking for .so's. No point in doing that since we are
#linking .a's only
#-MD produces the .d files

#Optionally include the symbol table for debugging. The odd versions of -g are
#to make sure that preprocessor information is also included (macros).
ifdef DEBUG
	CFLAGS+=-g3 -gdwarf-2 -O0
endif

#Linker options
LDFLAGS=-Wl,--gc-sections \
			  -Wl,--entry=Reset_EXCP \
				-Wl,-Tlink.ld \
				-Wl,-lm,-lc,-lgcc

#Define objcopy to extract out elf headers from binaries. Bare metal code does
#not have the ability to read these properly and will try to execute them which
#will likely cause undefined instruction errors.
OBJCOPY=arm-none-eabi-objcopy
OBJCFLAGS=-O binary
#C object files. This same effect could also be achieved with $@
OBJECTS=vectors.o init.o hw.o handlers.o

tm4c_os.bin: tm4c_os.elf
	${OBJCOPY} ${OBJCFLAGS} tm4c_os.elf tm4c_os.bin

tm4c_os.elf: ${OBJECTS} link.ld
	${CC} ${CFLAGS} ${LDFLAGS} -otm4c_os.elf ${OBJECTS}

init.o: init.c init.h hw.h
	${CC} ${CFLAGS} -c init.c

hw.o:	hw.c
	${CC} ${CFLAGS} -c hw.c

handlers.o: handlers.c
	${CC} ${CFLAGS} -c handlers.c

vectors.o: vectors.s
	${CC} ${CFLAGS} -c vectors.s

flash:
	lm4flash -s 0E2222BA -S 0x00000000 tm4c_os.bin

clean:
	rm -rf *.o *.d tm4c_os.map tm4c_os.elf tm4c_os.bin
