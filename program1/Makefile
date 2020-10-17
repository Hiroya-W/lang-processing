CC := gcc
OBJS := id-list.o token-list.o scan.o
SRC := id-list.c token-list.c scan.c
TEST_OBJS := test.o
CFLAGS := -ansi -fno-common -W -Wall -g 
TEST_CFLAGS := $(CFLAGS) -Dmain=_main_disabled -coverage -fprofile-arcs -ftest-coverage
TEST_LIBDIR := -L/usr/lib 
TEST_LIB := -lcunit

all: clean token-list test

token-list: $(OBJS)

test: test.c
	$(CC) $^ $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -o $@

test-ignore: test.c
	$(CC) $^ $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -Wno-missing-prototypes -static-libgcc -Wl,--unresolved-symbols=ignore-all,-zmuldefs -o $@

$(OBJS): token-list.h 

$(TEST_OBJS): token-list.h 

.PHONY: check
check: test
	@-rm *.gcno *.gcov *.gcda *.gch
	@./test

.PHONY: clean
clean:
	-rm *.o 
	-rm token-list test test-ignore
	-rm *.gcno *.gcov *.gcda *.gch

.DEFAULT_GOAL=all
