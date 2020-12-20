CC := gcc
OBJS := main.o scan.o cross_reference.o id-list.o
TEST_OBJS := test.o
SRC := main.c scan.c cross_reference.c id-list.c
CFLAGS := -ansi -fno-common -W -Wall -g 
TEST_CFLAGS := -fno-common -W -Wall -g -Dmain=_main_disabled -coverage -fprofile-arcs -ftest-coverage
TEST_LIBDIR := -L/usr/lib 
TEST_LIB := -lcunit

all: main test

main: $(OBJS)

test: test.c $(SRC)
	$(CC) $< $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -o $@

$(OBJS): mppl_compiler.h 

$(TEST_OBJS): mppl_compiler.h

.PHONY: check
check: test
	@./test

.PHONY: clean
clean:
	-rm *.o 
	-rm main test
	-rm *.gcno *.gcov *.gcda *.gch

.DEFAULT_GOAL=all
