CC := gcc
OBJS := main.o scan.o cross_reference.o id-list.o
SRC := main.c scan.c cross_reference.c id-list.c
CFLAGS := -ansi -fno-common -W -Wall -g 

all: main

main: $(OBJS)

$(OBJS): mppl_compiler.h 

.PHONY: clean
clean:
	-rm *.o 
	-rm main 

.DEFAULT_GOAL=all