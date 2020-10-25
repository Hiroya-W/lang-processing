#include "pretty-printer.h"

#include <stdio.h>

/*! String of each token */
char *tokenstr[NUMOFTOKEN + 1] = {
    "", "NAME", "program", "var", "array", "of", "begin",
    "end", "if", "then", "else", "procedure", "return", "call",
    "while", "do", "not", "or", "div", "and", "char",
    "integer", "boolean", "readln", "writeln", "true", "false", "NUMBER",
    "STRING", "+", "-", "*", "=", "<>", "<",
    "<=", ">", ">=", "(", ")", "[", "]",
    ":=", ".", ",", ":", ";", "read", "write",
    "break"};

static int parse_block(void);
static int parse_variable_declaration(void);
static int parse_variable_names(void);
static int parse_compound_statement(void);
static int parse_type(void);
static int parse_subprogram_declaration(void);

/*!
 * @brief Parsing a program
 * @return int Returns 0 on success and 1 on failure.
 */
int parse_program(void) {
    if (token != TPROGRAM) {
        return error("Keyword 'program' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);

    token = scan();
    if (token != TNAME) {
        return error("Program name is not found.");
    }
    fprintf(stdout, "%s", string_attr);

    token = scan();
    if (token != TSEMI) {
        return error("Semicolon is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    fprintf(stdout, "\n");

    token = scan();
    if (parse_block() == ERROR) {
        return ERROR;
    }

    if (token != TDOT) {
        return error("Period is not found at the end of program.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);

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
            if (parse_subprogram_declaration() == ERROR) {
                return ERROR;
            }
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
    fprintf(stdout, "%s ", tokenstr[token]);

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

        if (token != TCOLON) {
            return error("Colon is not found.");
        }
        fprintf(stdout, "%s ", tokenstr[token]);

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
    fprintf(stdout, "%s", string_attr);

    token = scan();
    while (token == TCOMMA) {
        fprintf(stdout, "%s ", tokenstr[token]);

        token = scan();
        if (token != TNAME) {
            return error("Name is not found.");
        }
        fprintf(stdout, "%s ", string_attr);
        token = scan();
    }
    return NORMAL;
}

static int parse_compound_statement(void) {
    return error("Unimplemented");
}

static int parse_type(void) {
    return error("Unimplemented");
}

static int parse_subprogram_declaration(void) {
    return error("Unimplemented");
}
