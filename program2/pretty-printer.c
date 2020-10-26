#include "pretty-printer.h"

#include <stdio.h>
#include <string.h>

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
static int parse_standard_type(void);
static int parse_array_type(void);
static int parse_subprogram_declaration(void);

static int parse_statement(void);
static int parse_assignment_statement(void);
static int parse_condition_statement(void);
static int parse_iteration_statement(void);
static int parse_exit_statement(void);
static int parse_call_statement(void);
static int parse_return_statement(void);
static int parse_input_statement(void);
static int parse_output_statement(void);
static int parse_output_format(void);
static int parse_compound_statement(void);

static int parse_expression(void);

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
            /* paragraph */
            if (parse_variable_declaration() == ERROR) {
                return ERROR;
            }
        } else {
            /* paragraph */
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
        token = scan();

        if (parse_type() == ERROR) {
            return ERROR;
        }

        if (token != TSEMI) {
            return error("Symbol ';' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }
    fprintf(stdout, "\n");
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

/*!
 * @brief Parsing a type
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_type(void) {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        if (parse_standard_type() == ERROR) {
            return ERROR;
        }
    } else if (token == TARRAY) {
        if (parse_array_type() == ERROR) {
            return ERROR;
        }
    } else {
        return error("Type is not found.");
    }
    return NORMAL;
}

/*!
 * @brief Parsing a standard type
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_standard_type(void) {
    if (token != TINTEGER && token != TBOOLEAN && token != TCHAR) {
        return error("Standard type is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing a array type
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_array_type(void) {
    if (token != TARRAY) {
        return error("Keyword 'array' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (token != TLSQPAREN) {
        return error("Symbol '[' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (token != TNUMBER) {
        return error("Number is not found.");
    }
    fprintf(stdout, "%s", string_attr);
    token = scan();

    if (token != TRSQPAREN) {
        return error("Symbol ']' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (token != TOF) {
        return error("Keyword 'of' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (parse_standard_type() == ERROR) {
        return ERROR;
    }
    return NORMAL;
}

/*!
 * @brief Parsing a subprogram declaration
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_subprogram_declaration(void) {
    return error("Unimplemented");
}

/*!
 * @brief Parsing a compound_statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_compound_statement(void) {
    if (token != TBEGIN) {
        return error("Keyword 'begin' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();
    /* paragraph */

    if (parse_statement() == ERROR) {
        return ERROR;
    }

    while (token == TSEMI) {
        fprintf(stdout, "%s", tokenstr[token]);
        fprintf(stdout, "\n");
        token = scan();

        if (parse_statement() == ERROR) {
            return ERROR;
        }
    }

    if (token != TEND) {
        return error("Keyword 'end' is not found.");
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing a subprogram declaration
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_statement(void) {
    switch (token) {
        case TNAME:
            if (parse_assignment_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TIF:
            if (parse_condition_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TWHILE:
            if (parse_iteration_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TBREAK:
            if (parse_exit_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TCALL:
            if (parse_call_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TRETURN:
            if (parse_return_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TREAD:
            /* FALLTHROUGH */
        case TREADLN:
            if (parse_input_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TWRITE:
            /* FALLTHROUGH */
        case TWRITELN:
            if (parse_output_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TBEGIN:
            if (parse_compound_statement() == ERROR) {
                return ERROR;
            }
            break;
        default:
            break;
    }

    return NORMAL;
}

static int parse_assignment_statement(void) {
    return error("Unimplemented");
}

static int parse_condition_statement(void) {
    return error("Unimplemented");
}
static int parse_iteration_statement(void) {
    return error("Unimplemented");
}
static int parse_exit_statement(void) {
    return error("Unimplemented");
}
static int parse_call_statement(void) {
    return error("Unimplemented");
}
static int parse_return_statement(void) {
    return error("Unimplemented");
}
static int parse_input_statement(void) {
    return error("Unimplemented");
}
static int parse_output_statement(void) {
    if (token != TWRITE && token != TWRITELN) {
        return error("Keyword 'write' or 'writeln' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (token == TLPAREN) {
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if (parse_output_format() == ERROR) {
            return ERROR;
        }

        while (token == TCOMMA) {
            fprintf(stdout, "%s ", tokenstr[token]);
            token = scan();

            if (parse_output_format() == ERROR) {
                return ERROR;
            }
        }

        if (token != TRPAREN) {
            return error("Symbol ')' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }
    return NORMAL;
}

static int parse_output_format(void) {
    if (token == TSTRING && strlen(string_attr) > 1) {
        fprintf(stdout, "'%s'", string_attr);
        token = scan();
        return NORMAL;
    }

    switch (token) {
        case TPLUS:
            /* FALLTHROUGH */
        case TMINUS:
            /* FALLTHROUGH */
        case TNAME:
            /* FALLTHROUGH */
        case TNUMBER:
            /* FALLTHROUGH */
        case TFALSE:
            /* FALLTHROUGH */
        case TTRUE:
            /* FALLTHROUGH */
        case TLPAREN:
            /* FALLTHROUGH */
        case TNOT:
            /* FALLTHROUGH */
        case TINTEGER:
            /* FALLTHROUGH */
        case TBOOLEAN:
            /* FALLTHROUGH */
        case TCHAR:
            if (parse_expression() == ERROR) {
                return ERROR;
            }

            if (token == TCOLON) {
                fprintf(stdout, "%s ", tokenstr[token]);
                token = scan();

                if (token != TNUMBER) {
                    return error("Number is not found.");
                }
                fprintf(stdout, "%s ", string_attr);
                token = scan();
            }
            break;
        default:
            return error("Output format is not found.");
    }
    return NORMAL;
}

static int parse_expression(void) {
    return error("Unimplemented");
}
