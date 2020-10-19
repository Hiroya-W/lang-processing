#include "pretty-printer.h"

int block();

int parse_program(void) {
    if (token != TPROGRAM) {
        error("Keyword 'program' is not found");
        return EXIT_FAILURE;
    }
    token = scan();
    if (token != TNAME) {
        error("Program name is not found");
        return EXIT_FAILURE;
    }
    token = scan();
    if (token != TSEMI) {
        error("Semicolon is not found");
        return EXIT_FAILURE;
    }
    token = scan();
    if (block() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    if (token != TDOT) {
        error("Period is not found at the end of program");
        return EXIT_FAILURE;
    }
    token = scan();
    return EXIT_SUCCESS;
}

int block() {
    return EXIT_SUCCESS;
}
