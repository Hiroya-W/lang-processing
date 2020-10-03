CC := gcc
OBJS := id-list.o token-list.o scan.o
CFLAGS := -ansi -W -Wall -g 

token-list: $(OBJS)

$(OBJS): token-list.h

.PHONY: clean
clean:
	-rm *.o token-list

.DEFAULT_GOAL=token-list
