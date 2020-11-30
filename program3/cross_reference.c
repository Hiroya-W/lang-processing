#include <CUnit/CUError.h>

#include "mppl_compiler.h"

static int parse_block(void);
static int parse_variable_declaration(void);
static int parse_variable_names(void);
static int parse_compound_statement(void);
static int parse_type(void);
static int parse_standard_type(void);
static int parse_array_type(void);
static int parse_subprogram_declaration(void);
static int parse_procedure_name(void);
static int parse_formal_parameters(void);

static int parse_statement(void);
static int parse_assignment_statement(void);
static int parse_condition_statement(void);
static int parse_iteration_statement(void);
static int parse_call_statement(void);

static int parse_variable(void);

static int parse_input_statement(void);
static int parse_output_statement(void);
static int parse_output_format(void);
static int parse_compound_statement(void);

static int parse_expression(void);
static int parse_simple_expression(void);
static int is_relational_operator(int token);
static int parse_term(void);
static int parse_factor(void);
static int parse_constant(void);
static int parse_expressions(void);

static void insert_indent(void);

/*! When a blank line exists, it becomes 1. */
static int exists_empty_statement = 0;
/*! Indicates the current indent level. */
static int indent_level = 0;
/*! Indicates a nesting level of while statement */
static int while_statement_level = 0;

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
    fprintf(stdout, "%s", tokenstr[token]);
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
    int is_the_first_line = 1;
    if (token != TVAR) {
        return error("Keyword 'var' is not found.");
    }

    insert_indent();
    fprintf(stdout, "%s ", tokenstr[token]);

    token = scan();

    while (token == TNAME) {
        if (is_the_first_line == 0) {
            /* insert tab */
            indent_level++;
            insert_indent();
        }

        if (parse_variable_names() == ERROR) {
            return ERROR;
        }

        if (token != TCOLON) {
            return error("Colon is not found.");
        }
        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if (parse_type() == ERROR) {
            return ERROR;
        }

        if (token != TSEMI) {
            return error("Symbol ';' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        fprintf(stdout, "\n");
        token = scan();

        if (is_the_first_line == 1) {
            is_the_first_line = 0;
        } else {
            indent_level--;
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
        fprintf(stdout, "%s", string_attr);
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
    fprintf(stdout, "%s", tokenstr[token]);
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
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (token != TOF) {
        return error("Keyword 'of' is not found.");
    }
    fprintf(stdout, " %s ", tokenstr[token]);
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
    if (token != TPROCEDURE) {
        return error("Keyword 'procedure' is not found.");
    }

    indent_level++;
    insert_indent();
    fprintf(stdout, "%s ", tokenstr[token]);

    token = scan();

    if (parse_procedure_name() == ERROR) {
        return ERROR;
    }

    if (token == TLPAREN && parse_formal_parameters() == ERROR) {
        return ERROR;
    }

    if (token != TSEMI) {
        return error("Symbol ';' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();

    if (token == TVAR) {
        if (parse_variable_declaration() == ERROR) {
            return ERROR;
        }
    }

    if (parse_compound_statement() == ERROR) {
        return ERROR;
    }

    if (token != TSEMI) {
        return error("Symbol ';' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();
    indent_level--;

    return NORMAL;
}

/*!
 * @brief Parsing a procedure name
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_procedure_name(void) {
    if (token != TNAME) {
        return error("Procedure name is not found.");
    }
    fprintf(stdout, "%s", string_attr);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing formal parameters
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_formal_parameters(void) {
    if (token != TLPAREN) {
        return error("Symbol '(' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (parse_variable_names() == ERROR) {
        return ERROR;
    }

    if (token != TCOLON) {
        return error("Symbol ':' is not found.");
    }
    fprintf(stdout, " %s ", tokenstr[token]);
    token = scan();

    if (parse_type() == ERROR) {
        return ERROR;
    }

    while (token == TSEMI) {
        fprintf(stdout, "%s", tokenstr[token]);
        fprintf(stdout, "\n");
        indent_level++;
        insert_indent();

        token = scan();

        if (parse_variable_names() == ERROR) {
            return ERROR;
        }

        if (token != TCOLON) {
            return error("Symbol ':' is not found.");
        }
        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if (parse_type() == ERROR) {
            return ERROR;
        }
        indent_level--;
    }

    if (token != TRPAREN) {
        return error("Sybmol ')' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing a compound_statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_compound_statement(void) {
    if (token != TBEGIN) {
        return error("Keyword 'begin' is not found.");
    }
    /*fprintf(stdout, "\r"); */
    insert_indent();
    fprintf(stdout, "%s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();

    indent_level++;
    insert_indent();

    if (parse_statement() == ERROR) {
        return ERROR;
    }

    while (token == TSEMI) {
        fprintf(stdout, "%s", tokenstr[token]);
        fprintf(stdout, "\n");
        token = scan();

        if (token != TEND) {
            insert_indent();
        }

        if (parse_statement() == ERROR) {
            return ERROR;
        }
    }

    if (token != TEND) {
        return error("Keyword 'end' is not found.");
    }

    if (exists_empty_statement) {
        /* fprintf(stdout, "\r");  "\r": Return to the top of the line */
        exists_empty_statement = 0;
    } else {
        fprintf(stdout, "\n");
    }
    indent_level--;

    insert_indent();
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing a statement
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
            if (while_statement_level == 0) {
                error("Keyword 'break' is written outside of a while statement.");
                return ERROR;
            }
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();
            break;
        case TCALL:
            if (parse_call_statement() == ERROR) {
                return ERROR;
            }
            break;
        case TRETURN:
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();
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
            /* There is no error here. */
            /* empty statement */
            exists_empty_statement = 1;
            break;
    }

    return NORMAL;
}

/*!
 * @brief Parsing a assignment statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_assignment_statement(void) {
    if (parse_variable() == ERROR) {
        return ERROR;
    }

    if (token != TASSIGN) {
        return error("Symbol ':=' is not found.");
    }
    fprintf(stdout, " %s ", tokenstr[token]);
    token = scan();

    if (parse_expression() == ERROR) {
        return ERROR;
    }

    return NORMAL;
}

/*!
 * @brief Parsing a condition statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_condition_statement(void) {
    if (token != TIF) {
        return error("Keyword 'if' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (parse_expression() == ERROR) {
        return ERROR;
    }

    if (token != TTHEN) {
        return error("Keyword 'then' is not found.");
    }
    fprintf(stdout, " %s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();

    indent_level++;

    if (token != TBEGIN) {
        insert_indent();
    }

    if (parse_statement() == ERROR) {
        return ERROR;
    }

    indent_level--;

    if (token == TELSE) {
        fprintf(stdout, "\n");
        insert_indent();

        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if (token != TIF) {
            fprintf(stdout, "\n");
            indent_level++;
            if (token != TBEGIN) {
                insert_indent();
            }
            if (parse_statement() == ERROR) {
                return ERROR;
            }
            indent_level--;
        } else {
            fprintf(stdout, " ");
            if (parse_statement() == ERROR) {
                return ERROR;
            }
        }
    }

    return NORMAL;
}

/*!
 * @brief Parsing a iteration statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_iteration_statement(void) {
    if (token != TWHILE) {
        return error("Keyword 'while' is not found.");
    }
    while_statement_level++;
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (parse_expression() == ERROR) {
        return ERROR;
    }

    if (token != TDO) {
        return error("Keyword 'do' is not found.");
    }
    fprintf(stdout, " %s", tokenstr[token]);
    fprintf(stdout, "\n");
    token = scan();

    if (parse_statement() == ERROR) {
        return ERROR;
    }
    while_statement_level--;
    return NORMAL;
}

/*!
 * @brief Parsing a call statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_call_statement(void) {
    if (token != TCALL) {
        return error("Keyword 'call' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if (parse_procedure_name() == ERROR) {
        return ERROR;
    }

    if (token == TLPAREN) {
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if (parse_expressions() == ERROR) {
            return ERROR;
        }

        if (token != TRPAREN) {
            return error("Symbol ')' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }

    return NORMAL;
}

/*!
 * @brief Parsing expressions
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_expressions(void) {
    if (parse_expression() == ERROR) {
        return ERROR;
    }

    while (token == TCOMMA) {
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan();

        if (parse_expression() == ERROR) {
            return ERROR;
        }
    }
    return NORMAL;
}

/*!
 * @brief Parsing a variable
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_variable(void) {
    if (token != TNAME) {
        return error("Name is not found.");
    }
    fprintf(stdout, "%s", string_attr);
    token = scan();

    return NORMAL;
}

/*!
 * @brief Parsing a input statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_input_statement(void) {
    if (token != TREAD && token != TREADLN) {
        return error("Keyword 'read' or 'readln' is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (token == TLPAREN) {
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if (parse_variable() == ERROR) {
            return ERROR;
        }

        while (token == TCOMMA) {
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if (parse_variable() == ERROR) {
                return ERROR;
            }
        }
        if (token != TRPAREN) {
            return error("Sybmol ')' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }

    return NORMAL;
}

/*!
 * @brief Parsing a output statement
 * @return int Returns 0 on success and 1 on failure.
 */
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

/*!
 * @brief Parsing a output format
 * @return int Returns 0 on success and 1 on failure.
 */
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
        case TSTRING:
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
                fprintf(stdout, " %s ", tokenstr[token]);
                token = scan();

                if (token != TNUMBER) {
                    return error("Number is not found.");
                }
                fprintf(stdout, "%s", string_attr);
                token = scan();
            }
            break;
        default:
            return error("Output format is not found.");
    }
    return NORMAL;
}

/*!
 * @brief Parsing a expression
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_expression(void) {
    if (parse_simple_expression() == ERROR) {
        return ERROR;
    }

    while (is_relational_operator(token)) {
        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if (parse_simple_expression() == ERROR) {
            return ERROR;
        }
    }

    return NORMAL;
}

/*!
 * @brief Determine if a token is a relational operator or not.
 * @param[in] _token Token to be determined
 * @return int Returns 1 for a relational operator, 0 otherwise.
 */
static int is_relational_operator(int _token) {
    switch (_token) {
        case TEQUAL:
            /* FALLTHROUGH */
        case TNOTEQ:
            /* FALLTHROUGH */
        case TLE:
            /* FALLTHROUGH */
        case TLEEQ:
            /* FALLTHROUGH */
        case TGR:
            /* FALLTHROUGH */
        case TGREQ:
            return 1;
        default:
            return 0;
    }
}

/*!
 * @brief Parsing a simple expression
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_simple_expression(void) {
    if (token == TPLUS || token == TMINUS) {
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }

    if (parse_term() == ERROR) {
        return ERROR;
    }

    while (token == TPLUS || token == TMINUS || token == TOR) {
        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if (parse_term() == ERROR) {
            return ERROR;
        }
    }
    return NORMAL;
}

/*!
 * @brief Parsing a term
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_term(void) {
    if (parse_factor() == ERROR) {
        return ERROR;
    }

    while (token == TSTAR || token == TDIV || token == TAND) {
        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if (parse_factor() == ERROR) {
            return ERROR;
        }
    }
    return NORMAL;
}

/*!
 * @brief Parsing a factor
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_factor(void) {
    switch (token) {
        case TNAME:
            if (parse_variable() == ERROR) {
                return ERROR;
            }
            break;
        case TNUMBER:
            /* FALLTHROUGH */
        case TFALSE:
            /* FALLTHROUGH */
        case TTRUE:
            /* FALLTHROUGH */
        case TSTRING:
            if (parse_constant() == ERROR) {
                return ERROR;
            }
            break;
        case TLPAREN:
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if (parse_expression() == ERROR) {
                return ERROR;
            }

            if (token != TRPAREN) {
                return error("Symbol ')' is not found.");
            }
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();
            break;
        case TNOT:
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if (parse_factor() == ERROR) {
                return ERROR;
            }
            break;
        case TINTEGER:
            /* FALLTHROUGH */
        case TBOOLEAN:
            /* FALLTHROUGH */
        case TCHAR:
            if (parse_standard_type() == ERROR) {
                return ERROR;
            }

            if (token != TLPAREN) {
                return error("Symbol '(' is not found");
            }
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if (parse_expression() == ERROR) {
                return ERROR;
            }

            if (token != TRPAREN) {
                return error("Symbol ')' is not found.");
            }
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();
            break;
        default:
            return error("Factor is not found.");
    }
    return NORMAL;
}

/*!
 * @brief Parsing a constant
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_constant(void) {
    switch (token) {
        case TNUMBER:
            fprintf(stdout, "%s", string_attr);
            break;
        case TFALSE:
            /* FALLTHROUGH */
        case TTRUE:
            fprintf(stdout, "%s", tokenstr[token]);
            break;
        case TSTRING:
            fprintf(stdout, "'%s'", string_attr);
            break;
        default:
            return error("Constant is not found.");
    }
    token = scan();
    return NORMAL;
}

/*!
 * @brief Insert an indent
 */
static void insert_indent(void) {
    int i;
    for (i = 0; i < indent_level; i++) {
        fprintf(stdout, "    ");
    }
    return;
}
