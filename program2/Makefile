CC := gcc
OBJS := main.o scan.o
SRC := main.c scan.c
CFLAGS := -ansi -fno-common -W -Wall -g 

all: clean main

main: $(OBJS)

$(OBJS): pretty-printer.h 

.PHONY: clean
clean:
	-rm *.o 
	-rm main

.DEFAULT_GOAL=all