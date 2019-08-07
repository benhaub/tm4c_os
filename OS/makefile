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
#Flags for C compiler.
CFLAGS=-mthumb \
			 -nodefaultlibs \
			 -static \
			 ${CPU} \
			 ${FPU} \
			 -Iinclude \
			 -MD \
			 -ffunction-sections \
			 -fdata-sections \
			 -ffreestanding \
			 -std=c99 \
			 -Wall \
			 -pedantic
#-static: Stops ld from looking for .so's. No point in doing that since we are
#linking .a's only
#-MD produces the .d files

#Optionally include the symbol table for debugging. The 3 on the end of -g is
#the level. level 3 debugging symbols include everything up to preprocessors
#symbols. The -ggdb is the debugging symbol format. This one is specifically
#for gbd. -O0 eliminates compiler optomizations, so that the codes binary is
#the same as the way we wrote it. View page 120 of the GNU gcc docs for more
#info
ifdef DEBUG
	CFLAGS+=-g3 -ggdb -O0
endif

#Linker options
LDFLAGS=-Wl,--gc-sections \
			  -Wl,--entry=Reset_EXCP \
				-Wl,-Tlink.ld \
				-Wl,-lm,-lgcc

#Define objcopy to extract out elf headers from binaries. Bare metal code does
#not have the ability to read these properly and will try to execute them which
#will likely cause undefined instruction errors.
OBJCOPY=arm-none-eabi-objcopy
OBJCFLAGS=-O binary
#C object files.
OBJECTS=vectors.o init.o hw.o handlers.o proc.o mem.o cstring.o context.o \
				initshell.o syscalls.o syscallsasm.o kernel_services.o

tm4c_os.bin: tm4c_os.elf
	${OBJCOPY} ${OBJCFLAGS} tm4c_os.elf tm4c_os.bin

tm4c_os.elf: ${OBJECTS} link.ld
	${CC} ${CFLAGS} ${LDFLAGS} -otm4c_os.elf ${OBJECTS}

init.o: init.c hw.h mem.h cstring.h proc.h
	${CC} ${CFLAGS} -c init.c

hw.o:	hw.c hw.h
	${CC} ${CFLAGS} -c hw.c

handlers.o: handlers.c
	${CC} ${CFLAGS} -c handlers.c

vectors.o: vectors.s
	${CC} ${CFLAGS} -c vectors.s

mem.o: mem.c types.h mem.h
	${CC} ${CFLAGS} -c mem.c

proc.o: proc.c mem.h proc.h cstring.h
	${CC} ${CFLAGS} -c proc.c

cstring.o: cstring.c mem.h types.h
	${CC} ${CFLAGS} -c cstring.c

context.o: context.s
	${CC} ${CFLAGS} -c context.s

initshell.o: initshell.c syscalls.h hw.h proc.h
	${CC} ${CFLAGS} -c initshell.c

syscalls.o: syscalls.c syscalls.h types.h proc.h
	${CC} ${CFLAGS} -c syscalls.c

syscallsasm.o: syscallsasm.s
	${CC} ${CFLAGS} -c syscallsasm.s

kernel_services.o: kernel_services.c kernel_services.h proc.h types.h cstring.h
	${CC} ${CFLAGS} -c kernel_services.c

flash:
	lm4flash -s 0E2222BA -S 0x00000000 tm4c_os.bin

clean:
	rm -rf *.o *.d tm4c_os.map tm4c_os.elf tm4c_os.bin
