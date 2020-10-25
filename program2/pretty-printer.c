#include "pretty-printer.h"

static int parse_block(void);
static int parse_variable_declaration(void);
static int parse_variable_names(void);
static int parse_compound_statement(void);
static int parse_type(void);

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
    if (parse_block() == ERROR) {
        return ERROR;
    }
    if (token != TDOT) {
        return error("Period is not found at the end of program.");
    }
    token = scan();
    return NORMAL;
}

/*!
 * @brief Parsing a block
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_block(void) {
    while (token == TVAR || token == TPROCEDURE) {
        if (token == TVAR) {
            if (parse_variable_declaration() == ERROR) {
                return ERROR;
            }
        } else {
        }
    }
    if (parse_compound_statement() == ERROR) {
        return ERROR;
    }
    return NORMAL;
}

/*!
 * @brief Parsing a variable declaration
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_variable_declaration(void) {
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

        if (parse_variable_names() == ERROR) {
            return ERROR;
        }
        token = scan();
        if (token != TCOLON) {
            return error("Colon is not found.");
        }
        if (parse_type() == ERROR) {
            return ERROR;
        }
    }
    return NORMAL;
}

/*!
 * @brief Parsing a variable names
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_variable_names(void) {
    if (token != TNAME) {
        return error("Name is not found.");
    }
    token = scan();
    while (token == TCOMMA) {
        token = scan();
        if (token != TNAME) {
            return error("Name is not found.");
        }
    }
    return NORMAL;
}
static int parse_compound_statement(void) {
    return ERROR;
}
static int parse_type(void) {
    return ERROR;
}
