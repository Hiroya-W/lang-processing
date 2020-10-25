#include "pretty-printer.h"

static int block(void);
static int variable_declaration(void);
static int variable_names(void);
static int hukugoubun(void);
static int type(void);

/*!
 * @brief Parsing a program
 * @return int Returns 0 on success and 1 on failure.
 */
int parse_program(void) {
    if (token != TPROGRAM) {
        return error("Keyword 'program' is not found.");
    }
    token = scan();
    if (token != TNAME) {
        return error("Program name is not found.");
    }
    token = scan();
    if (token != TSEMI) {
        return error("Semicolon is not found.");
    }
    token = scan();
    if (block() == ERROR) {
        return ERROR;
    }
    if (token != TDOT) {
        return error("Period is not found at the end of program.");
    }
    token = scan();
    return NORMAL;
}

/*!
 * @brief Parsing a variable declaration
 * @return int Returns 0 on success and 1 on failure.
 */
static int variable_declaration(void) {
    int is_the_first_line = 0;
    if (token != TVAR) {
        return error("Keyword 'var' is not found.");
    }

    token = scan();
    while (token == TNAME) {
        if (is_the_first_line == 0) {
            is_the_first_line = 1;
        } else {
            /* insert tab */
        }

        if (variable_names() == ERROR) {
            return ERROR;
        }
        token = scan();
        if (token != TCOLON) {
            return error("Colon is not found.");
        }
        if (type() == ERROR) {
            return ERROR;
        }
    }
    return NORMAL;
}

/*!
 * @brief Parsing a block
 * @return int Returns 0 on success and 1 on failure.
 */
static int block(void) {
    /* 
     * variable declaration : TVAR
     * subprogram declaration : TPROCEDURE
     * composite statement : TBEGIN
     */
    while (token == TVAR || token == TPROCEDURE) {
        if (token == TVAR) {
            if (variable_declaration() == ERROR) {
                return ERROR;
            }
        } else {
        }
    }
    if (hukugoubun() == ERROR) {
        return ERROR;
    }
    return NORMAL;
}
