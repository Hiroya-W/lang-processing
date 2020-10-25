#include "pretty-printer.h"

int block();
int hukugoubun();

/*!
 * @brief Parsing a program
 * @return int Returns 0 on success and 1 on failure.
 */
int parse_program(void) {
    if (token != TPROGRAM) {
        error("Keyword 'program' is not found.");
        return ERROR;
    }
    token = scan();
    if (token != TNAME) {
        error("Program name is not found.");
        return ERROR;
    }
    token = scan();
    if (token != TSEMI) {
        error("Semicolon is not found.");
        return ERROR;
    }
    token = scan();
    if (block() == ERROR) {
        return ERROR;
    }
    if (token != TDOT) {
        error("Period is not found at the end of program.");
        return ERROR;
    }
    token = scan();
    return NORMAL;
}

/*!
 * @brief Parsing a block
 * @return int Returns 0 on success and 1 on failure.
 */
int block(void) {
    /* 
     * variable declaration : TVAR
     * subprogram declaration : TPROCEDURE
     * composite statement : TBEGIN
     */
    while (token == TVAR || token == TPROCEDURE) {
        if (token == TVAR) {
        } else {
        }
    }
    if (hukugoubun() == ERROR) {
        return ERROR;
    }
    return NORMAL;
}
