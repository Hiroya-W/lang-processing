CC := gcc
OBJS := id-list.o token-list.o
CFLAGS := -W -Wall -g

token-list: $(OBJS)

$(OBJS): token-list.h

.PHONY: clean
clean:
	-rm *.o 

.DEFAULT_GOAL=token-list
