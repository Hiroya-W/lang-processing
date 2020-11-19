CC := gcc
OBJS := main.o scan.o pretty-printer.o
TEST_OBJS := test.o
SRC := main.c scan.c pretty-printer.c
CFLAGS := -ansi -fno-common -W -Wall -g 
TEST_CFLAGS := $(CFLAGS) -Dmain=_main_disabled -coverage -fprofile-arcs -ftest-coverage
TEST_LIBDIR := -L/usr/lib 
TEST_LIB := -lcunit

all: main test

main: $(OBJS)

test: test.c
	$(CC) $^ $(TEST_CFLAGS) $(TEST_LIBDIR) $(TEST_LIB) -o $@

$(OBJS): mppl_compiler.h 

$(TEST_OBJS): mppl_compiler.h $(SRC)

.PHONY: check
check: test
	@mkdir -p outputs
	sh output_each_file.sh
	@./test
	@-rm -rf outputs

.PHONY: clean
clean:
	-rm *.o 
	-rm main test
	-rm *.gcno *.gcov *.gcda *.gch

.DEFAULT_GOAL=all
