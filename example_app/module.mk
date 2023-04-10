#Provide sources and includes relative to the top level make file
C_SOURCES += example_app/main.c
INCLUDES += -Iexample_app/include
#Libs would be include like this:
#LIBS += -lmylib
#Add to CFLAGS if you need to add library paths
#CFLAGS += example_app/libs
#If you have any asm files:
#S_SOURCES += myasm.s
