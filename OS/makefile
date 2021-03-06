###############################################################################
#Authour  : Ben Haubrich                                                      #
#File     : makefile                                                          #
#Synopsis : makefile for tm4c_os                                              #
###############################################################################

#Search for header files here as well as the current working directory
VPATH=include
#Define the C compiler
CC=arm-none-eabi-gcc
#Architecture specific options for C compiler
CPU=-mcpu=cortex-m4 -march=armv7e-m -mtune=cortex-m4
#The FPU is single precision (sp) with 16 double word registers (d16). The
#important information here is the sp. C treats all floats as
#doubles. They must be forced to single precision.
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=hard
#Flags for C compiler.
CFLAGS=-mthumb \
        ${CPU} \
        ${FPU} \
       -Iinclude \
       -MD \
       -MF ./.deps/$*.d \
       -ffunction-sections \
       -fdata-sections \
       -ffreestanding \
			 -fsingle-precision-constant \
       -std=c99 \
       -nodefaultlibs \
       -static \
       -pedantic \
       -Wall
#-static: Stops ld from looking from .so's. No point in doing that since we are
#linking .a's only
#-M produces the .d files, -MF specifies an output directory for them
#The $* is called an automatic variable. The $* becomes whatever the wildcard
#value expands to in the targets below in the implicit included rules.

#Linker options
LDFLAGS=-Wl,--gc-sections \
        -Wl,--entry=Reset_EXCP \
        -Wl,-Tlink.ld \
				-Wl,-Map=link.map \
        -Wl,-lgcc

#Optionally include the symbol table for debugging. The 3 on the end of -g is
#the level. level 3 debugging symbols include everything up to preprocessor
#symbols. The -ggdb is the debugging symbol format. This one is specifically
#for gbd. -O0 eliminates compiler optomizations, so that the compiled code is
#the same as the text code. View page 120 of the GNU gcc docs for more
#info
ifdef DEBUG
  CFLAGS+=-g3 -ggdb -O0
endif

#Define objcopy to extract out elf headers from binaries. Bare metal code does
#not have the ability to read these properly and will try to execute them which
#will likely cause undefined instruction errors.
OBJCOPY=arm-none-eabi-objcopy
OBJCFLAGS=-O binary
C_SOURCES=$(wildcard *.c)
S_SOURCES=$(wildcard *.s)
#C object files. This type of variable makes use of subsitution references
C_OBJECTS=${C_SOURCES:.c=.o}
S_OBJECTS+=${S_SOURCES:.s=.o}

.PHONY: flash clean dirs

tm4c_os.bin: dirs tm4c_os.elf
	${OBJCOPY} ${OBJCFLAGS} tm4c_os.elf tm4c_os.bin
	mv *.elf *.o .bin

tm4c_os.elf: ${C_OBJECTS} ${S_OBJECTS} link.ld
	${CC} ${CFLAGS} ${LDFLAGS} -otm4c_os.elf ${C_OBJECTS} ${S_OBJECTS}

vectors.o: vectors.s
	${CC} ${CFLAGS} -c vectors.s

context.o: context.s
	${CC} ${CFLAGS} -c context.s

syscallsasm.o: syscallsasm.s
	${CC} ${CFLAGS} -c syscallsasm.s

dirs:
	mkdir -p .deps .bin

#If a dependancy file is deleted for some reason, make will detect it and
#rebuild them.
./.deps/%.d: ;

# The dependancy files generated by gcc are read as implicit rules
include $(wildcard ./.deps/*.d)

flash:
	lm4flash -S 0x00000000 tm4c_os.bin

clean:
	rm -rf *.o ./.deps tm4c_os.map tm4c_os.elf tm4c_os.bin
