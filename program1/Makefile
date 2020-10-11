CC := gcc
OBJS := id-list.o token-list.o scan.o
SRC := id-list.c token-list.c scan.c
TEST_OBJS := test.o
CFLAGS := -ansi -fno-common -W -Wall -g
TEST_CFLAGS := -ansi -fno-common -W -Wall -g -Dmain=_main_disabled
TEST_LIBDIR := -L/usr/lib 
TEST_LIB := -lbcunit

all: token-list test

token-list: $(OBJS)

test: test.c
	$(CC) $^ $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -o $@

test-ignore: test.c
	$(CC) $^ $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -Wno-missing-prototypes -static-libgcc -Wl,--unresolved-symbols=ignore-all,-zmuldefs -o $@

$(OBJS): token-list.h 

$(TEST_OBJS): token-list.h 

.PHONY: clean
clean:
	-rm *.o token-list test test-ignore

.DEFAULT_GOAL=all
