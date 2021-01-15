#include "mppl_compiler.h"

/*! @name Boolean value */
/* @{ */
/*! True */
#define true 1
/*! False */
#define false 0
/* @} */

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

static int parse_expression(int *is_expression_variable_only);
static int parse_simple_expression(int *is_simple_expression_variable_only);
static int is_relational_operator(int token);
static int parse_term(int *is_variable_only);
static int parse_factor(int *is_variable);
static int parse_constant(void);
static int parse_expressions(void);

static void insert_indent(void);

/*! When a blank line exists, it becomes 1. */
static int exists_empty_statement = 0;
/*! Indicates the current indent level. */
static int indent_level = 0;
/*! Indicates a nesting level of while statement */
static int while_statement_level = 0;
/*! When in subprogram declaration, it becomes 1 */
int in_subprogram_declaration = 0;
/*! When in variable declaration, it becomes 1 */
int in_variable_declaration = 0;
/*! When in call statement, it becomes 1 */
int in_call_statement = 0;
/*! if id's type is array, it becomes 1 */
int is_array_type = 0;
/*! if id is formal parameter, it becomes 1 */
int is_formal_parameter = 0;
/*! When defining procedure name, it becomes 1 */
int definition_procedure_name = 0;
/*! Pointer to id of procedure name*/
struct ID *id_procedure = NULL;
/*! Pointer to id of referenced variable */
struct ID *id_variable = NULL;
struct ID *id_referenced_variable;

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

    if (assemble_start(string_attr) == ERROR) {
        return ERROR;
    }

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

    assemble_literals();
    assemble_library();

    return NORMAL;
}

/*!
 * @brief Parsing a block
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_block(void) {
    char *L0001 = NULL;
    if (create_newlabel(&L0001) == ERROR) {
        return ERROR;
    }

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

    fprintf(out_fp, "%s\n", L0001);
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

    in_variable_declaration = true;

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

    in_variable_declaration = false;

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

    /* declaration */
    if (in_variable_declaration || is_formal_parameter) {
        if (id_register_without_type(string_attr) == ERROR) {
            return ERROR;
        }
    }
    /* reference */
    else if (register_linenum(string_attr) == ERROR) {
        return ERROR;
    }

    token = scan();

    while (token == TCOMMA) {
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan();

        if (token != TNAME) {
            return error("Name is not found.");
        }
        fprintf(stdout, "%s", string_attr);

        /* definition */
        if (in_variable_declaration || is_formal_parameter) {
            if (id_register_without_type(string_attr) == ERROR) {
                return ERROR;
            }
        }
        /* reference */
        else if (register_linenum(string_attr) == ERROR) {
            return ERROR;
        }

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
        if (is_formal_parameter) {
            return error("Array types cannot be defined in the formal parameter");
        }
        is_array_type = true;
        if (parse_array_type() == ERROR) {
            return ERROR;
        }
        is_array_type = false;
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
    int standard_type = TPNONE;
    struct TYPE *type;
    if (token != TINTEGER && token != TBOOLEAN && token != TCHAR) {
        return error("Standard type is not found.");
    }
    fprintf(stdout, "%s", tokenstr[token]);

    /* regist id */
    if (in_variable_declaration || is_formal_parameter) {
        if (is_array_type) {
            switch (token) {
                case TINTEGER:
                    type = array_type(TPARRAYINT);
                    break;
                case TBOOLEAN:
                    type = array_type(TPARRAYBOOL);
                    break;
                case TCHAR:
                    type = array_type(TPARRAYCHAR);
                    break;
            }
        } else {
            switch (token) {
                case TINTEGER:
                    type = std_type(TPINT);
                    break;
                case TBOOLEAN:
                    type = std_type(TPBOOL);
                    break;
                case TCHAR:
                    type = std_type(TPCHAR);
                    break;
            }
        }
        /* error multiple definition or can not malloc */
        if (id_register_as_type(&type) == ERROR) {
            return ERROR;
        }
    }

    switch (token) {
        case TINTEGER:
            standard_type = TPINT;
            break;
        case TBOOLEAN:
            standard_type = TPBOOL;
            break;
        case TCHAR:
            standard_type = TPCHAR;
            break;
    }

    token = scan();
    return standard_type;
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

    /* array size */
    if (in_variable_declaration) {
        /* The size of the array that can be defined is 1 <= 'array size' */
        if (num_attr < 1) {
            return error("The size of the array that can be defined is 1 <= 'array size'.");
        }
    }

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

    fprintf(stdout, "%s ", tokenstr[token]);

    token = scan();

    in_subprogram_declaration = true;
    definition_procedure_name = true;

    if (parse_procedure_name() == ERROR) {
        return ERROR;
    }

    definition_procedure_name = false;

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

    indent_level++;
    assemble_procedure_definition();
    assemble_procedure_begin();

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

    in_subprogram_declaration = false;
    release_localidroot();

    assemble_procedure_end();

    return NORMAL;
}

/*!
 * @brief Parsing a procedure name
 * @param[in] register_mode If 1, store the name of the procedure 
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_procedure_name(void) {
    if (token != TNAME) {
        return error("Procedure name is not found.");
    }
    fprintf(stdout, "%s", string_attr);

    /* definition */
    if (definition_procedure_name) {
        struct TYPE *type;
        /* regist procedure name */
        id_register_without_type(string_attr);
        /* procedure name's type is TPPROC */
        type = std_type(TPPROC);
        /* error multiple definition or can not malloc */
        if (id_register_as_type(&type) == ERROR) {
            return ERROR;
        }

        set_procedure_name(string_attr);
    }
    /* reference */
    else {
        if (register_linenum(string_attr) == ERROR) {
            return ERROR;
        }
        id_procedure = search_procedure(string_attr);
    }
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

    is_formal_parameter = true;

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

    is_formal_parameter = false;

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
    int var_type = TPNONE;
    int exp_type = TPNONE;
    int is_expression_variable_only = 0;

    if ((var_type = parse_variable()) == ERROR) {
        return ERROR;
    }

    if (token != TASSIGN) {
        return error("Symbol ':=' is not found.");
    }
    fprintf(stdout, " %s ", tokenstr[token]);
    token = scan();

    if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
        return ERROR;
    }

    if (is_expression_variable_only) {
        assemble_variable_reference_rval(id_referenced_variable);
    }

    if (var_type != exp_type) {
        return error("The types of the operand1 and operand2 do not match");
    }

    assemble_assign();

    return NORMAL;
}

/*!
 * @brief Parsing a condition statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_condition_statement(void) {
    int exp_type = TPNONE;
    int is_expression_variable_only = 0;
    char *else_label = NULL;
    char *if_end_label = NULL;

    if (token != TIF) {
        return error("Keyword 'if' is not found.");
    }
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
        return ERROR;
    }

    if (is_expression_variable_only) {
        /* condition needs right value */
        assemble_variable_reference_rval(id_referenced_variable);
    }

    if (exp_type != TPBOOL) {
        return error("The type of the condition must be boolean.");
    }

    if (create_newlabel(&else_label) == ERROR) {
        return ERROR;
    }
    assemble_if_condition(else_label);

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

    if (create_newlabel(&if_end_label) == ERROR) {
        return ERROR;
    }

    if (token == TELSE) {
        assemble_else(if_end_label, else_label);
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
        fprintf(out_fp, "%s\n", if_end_label);
    } else {
        fprintf(out_fp, "%s\n", else_label);
    }

    return NORMAL;
}

/*!
 * @brief Parsing a iteration statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_iteration_statement(void) {
    int exp_type = TPNONE;
    int is_expression_variable_only = 0;
    char *iteration_top_label = NULL;
    char *iteration_bottom_label = NULL;

    create_newlabel(&iteration_top_label);
    create_newlabel(&iteration_bottom_label);

    fprintf(out_fp, "%s\n", iteration_top_label);

    if (token != TWHILE) {
        return error("Keyword 'while' is not found.");
    }
    while_statement_level++;
    fprintf(stdout, "%s ", tokenstr[token]);
    token = scan();

    if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
        return ERROR;
    }

    if (exp_type != TPBOOL) {
        return error("The type of the condition must be boolean.");
    }

    if (is_expression_variable_only) {
        /* condition needs right value */
        assemble_variable_reference_rval(id_referenced_variable);
    }

    assemble_iteration_condition(iteration_bottom_label);

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

    fprintf(out_fp, "\tJUMP \t%s\n", iteration_top_label);
    fprintf(out_fp, "%s\n", iteration_bottom_label);

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

    in_call_statement = true;
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
    } else {
        struct TYPE *para_type = id_procedure->itp->paratp;
        if (para_type != NULL) {
            return error("There are a few arguments.");
        }
    }
    in_call_statement = false;

    assemble_call(id_procedure);

    return NORMAL;
}

/*!
 * @brief Parsing expressions
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_expressions(void) {
    /* It is always call statement. */
    int exp_type = TPNONE;
    int num_of_exp = 0;
    int is_expression_variable_only = 0;
    struct TYPE *para_type = id_procedure->itp->paratp;

    if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
        return ERROR;
    }

    num_of_exp++;

    if (in_call_statement) {
        if (para_type == NULL) {
            return error("This procedure takes no arguments.");
        }
        if (para_type->ttype != exp_type) {
            return error("The type of the argument1 does not match.");
        }

        if (is_expression_variable_only) {
            /* call by reference */
            assemble_variable_reference_lval(id_referenced_variable); /* address */
        } else {
            /* expression doesn't have address */
        }
    }

    while (token == TCOMMA) {
        fprintf(stdout, "%s ", tokenstr[token]);
        token = scan();

        if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
            return ERROR;
        }

        num_of_exp++;
        if (in_call_statement) {
            para_type = para_type->paratp;
            if (para_type == NULL) {
                return error("There are a lot of arguments.");
            }
            if (para_type->ttype != exp_type) {
                fprintf(stderr, "The type of the argument%d does not match.", num_of_exp);
                return error("The type of the argument does not match.");
            }

            if (is_expression_variable_only) {
                assemble_variable_reference_lval(id_referenced_variable); /* address */
            } else {
                /* expression doesn't have address */
            }
        }
    }

    if (in_call_statement) {
        if (para_type->paratp != NULL) {
            return error("There are a few arguments.");
        }
    }

    return NORMAL;
}

/*!
 * @brief Parsing a variable
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_variable(void) {
    int id_type = TPNONE;
    int is_expression_variable_only = 0;

    if (token != TNAME) {
        return error("Name is not found.");
    }
    fprintf(stdout, "%s", string_attr);

    if ((id_type = register_linenum(string_attr)) == ERROR) {
        return ERROR;
    }

    id_referenced_variable = id_variable;

    token = scan();

    if (token == TLSQPAREN) {
        int exp_type = TPNONE;
        if (!(id_type & TPARRAY)) {
            fprintf(stderr, "%s is not Array type.", string_attr);
            return error("id is not Array type.");
        }

        /* name is array type */
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
            return ERROR;
        } else if (exp_type != TPINT) {
            return error("The array index type must be an integer.");
        }

        if (is_expression_variable_only) {
            /* index need right value */
            assemble_variable_reference_rval(id_referenced_variable);
        }

        if (token != TRSQPAREN) {
            return error("Sybmol ']' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        /* the type of the array elements */
        switch (id_type) {
            case TPARRAYINT:
                id_type = TPINT;
                break;
            case TPARRAYCHAR:
                id_type = TPCHAR;
                break;
            case TPARRAYBOOL:
                id_type = TPBOOL;
                break;
        }
    }
    /*
    else {
        assemble_variable_reference_lval(id_referenced_variable);
    }
    */

    /* TODO: Is variable array type? */
    /* Is the address to be returned an array or a variable? */

    return id_type;
}

/*!
 * @brief Parsing a input statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_input_statement(void) {
    int read_token;

    if (token != TREAD && token != TREADLN) {
        return error("Keyword 'read' or 'readln' is not found.");
    }
    read_token = token;

    fprintf(stdout, "%s", tokenstr[token]);
    token = scan();

    if (token == TLPAREN) {
        int var_type = TPNONE;
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();

        if ((var_type = parse_variable()) == ERROR) {
            return ERROR;
        }

        if (var_type != TPINT && var_type != TPCHAR) {
            return error("The type of the variable must be integer or char.");
        }

        assemble_variable_reference_lval(id_referenced_variable);
        assemble_read(var_type);

        while (token == TCOMMA) {
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if ((var_type = parse_variable()) == ERROR) {
                return ERROR;
            }

            if (var_type != TPINT && var_type != TPCHAR) {
                return error("The type of the variable must be integer or char.");
            }

            assemble_variable_reference_lval(id_referenced_variable);
            assemble_read(var_type);
        }
        if (token != TRPAREN) {
            return error("Sybmol ')' is not found.");
        }
        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }

    if (read_token == TREADLN) {
        assemble_read_line();
    }

    return NORMAL;
}

/*!
 * @brief Parsing a output statement
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_output_statement(void) {
    int write_token;
    if (token != TWRITE && token != TWRITELN) {
        return error("Keyword 'write' or 'writeln' is not found.");
    }
    write_token = token;

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

    if (write_token == TWRITELN) {
        assemble_output_line();
    }

    return NORMAL;
}

/*!
 * @brief Parsing a output format
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_output_format(void) {
    int exp_type = TPNONE;
    int is_expression_variable_only = 0;

    if (token == TSTRING && strlen(string_attr) > 1) {
        fprintf(stdout, "'%s'", string_attr);

        if (assemble_output_format_string(string_attr) == ERROR) {
            return ERROR;
        }

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
            if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
                return ERROR;
            }

            if (is_expression_variable_only) {
                assemble_variable_reference_rval(id_referenced_variable);
            }

            if (exp_type & TPARRAY) {
                return error("The type must be a standard type.");
            }

            if (token == TCOLON) {
                fprintf(stdout, " %s ", tokenstr[token]);
                token = scan();

                if (token != TNUMBER) {
                    return error("Number is not found.");
                }
                fprintf(stdout, "%s", string_attr);
                token = scan();

                assemble_output_format_standard_type(exp_type, num_attr);
            } else {
                assemble_output_format_standard_type(exp_type, 0);
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
static int parse_expression(int *is_expression_variable_only) {
    int exp_type1 = TPNONE;
    int is_simple_expression_variable_only = 0;
    *is_expression_variable_only = true;

    if ((exp_type1 = parse_simple_expression(&is_simple_expression_variable_only)) == ERROR) {
        return ERROR;
    }

    if (!is_simple_expression_variable_only) {
        *is_expression_variable_only = false;
    }

    while (is_relational_operator(token)) {
        /* The type of the result of a relational operator is a boolean. */
        int exp_type2 = TPNONE;
        *is_expression_variable_only = false;

        if (is_simple_expression_variable_only) {
            assemble_variable_reference_rval(id_referenced_variable);
        }

        fprintf(stdout, " %s ", tokenstr[token]);
        token = scan();

        if ((exp_type2 = parse_simple_expression(&is_simple_expression_variable_only)) == ERROR) {
            return ERROR;
        }

        if (exp_type1 != exp_type2) {
            return error("The types of the operand1 and operand2 do not match");
        }

        /* The type of the result of a relational operator is a boolean. */
        exp_type1 = TPBOOL;

        if (is_simple_expression_variable_only) {
            assemble_variable_reference_rval(id_referenced_variable);
            is_simple_expression_variable_only = false;
        }

        assemble_expression();
    }

    return exp_type1;
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
static int parse_simple_expression(int *is_simple_expression_variable_only) {
    int term_type1 = TPNONE;
    int term_type2 = TPNONE;
    int opr;
    int is_term_variable_only = 0;
    *is_simple_expression_variable_only = true;

    if (token == TPLUS || token == TMINUS) {
        *is_simple_expression_variable_only = false;
        term_type1 = TPINT;

        fprintf(stdout, "%s", tokenstr[token]);
        token = scan();
    }

    if ((term_type2 = parse_term(&is_term_variable_only)) == ERROR) {
        return ERROR;
    }

    if (term_type1 == TPINT && term_type2 != TPINT) {
        /* if there are + or -, the type must be integer  */
        return error("The type of the term must be integer.");
    }
    term_type1 = term_type2;

    if (is_term_variable_only && !(*is_simple_expression_variable_only)) {
        /* if TPLUS or TMINUS exists, !is_simple_expression_variable_only is true*/
        /* Then need the right value to calculate */
        assemble_variable_reference_rval(id_referenced_variable);
        /* TODO: calculate puls or minus */

        is_term_variable_only = false;
    }

    if (!is_term_variable_only) {
        *is_simple_expression_variable_only = false;
    }

    while (token == TPLUS || token == TMINUS || token == TOR) {
        *is_simple_expression_variable_only = false;

        if (is_term_variable_only) {
            /* Load a right value from a variable to calculate */
            assemble_variable_reference_rval(id_referenced_variable);
        }

        fprintf(stdout, " %s ", tokenstr[token]);

        if ((token == TPLUS || token == TMINUS) && term_type1 != TPINT) {
            return error("The type of the operand must be integer.");
        } else if (token == TOR && term_type1 != TPBOOL) {
            return error("The type of the operand must be boolean.");
        }
        opr = token;

        token = scan();

        if ((term_type2 = parse_term(&is_term_variable_only)) == ERROR) {
            return ERROR;
        }

        if (term_type1 == TPINT && term_type2 != TPINT) {
            return error("The type of the operand must be integer.");
        } else if (term_type1 == TPBOOL && term_type2 != TPBOOL) {
            return error("The type of the operand must be boolean.");
        }

        if (is_term_variable_only) {
            /* Load a right value from a variable to calculate */
            assemble_variable_reference_rval(id_referenced_variable);
            is_term_variable_only = false;
        }

        if (opr == TPLUS) {
            assemble_ADDA();
        } else if (opr == TMINUS) {
            assemble_SUBA();
        } else if (opr == TOR) {
            assemble_OR();
        }
    }
    return term_type1;
}

/*!
 * @brief Parsing a term
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_term(int *is_variable_only) {
    int term_type1 = TPNONE;
    int term_type2 = TPNONE;
    int opr;
    int is_variable = 0;
    *is_variable_only = true;

    if ((term_type1 = parse_factor(&is_variable)) == ERROR) {
        return ERROR;
    }

    if (!is_variable) {
        *is_variable_only = false;
    }

    while (token == TSTAR || token == TDIV || token == TAND) {
        *is_variable_only = false;
        if (is_variable) {
            /* Load a right value from a variable to calculate */
            assemble_variable_reference_rval(id_referenced_variable);
        }

        fprintf(stdout, " %s ", tokenstr[token]);

        if ((token == TSTAR || token == TDIV) && term_type1 != TPINT) {
            return error("The type of the operand must be integer.");
        } else if (token == TAND && term_type1 != TPBOOL) {
            return error("The type of the operand must be boolean.");
        }
        opr = token;

        token = scan();

        if ((term_type2 = parse_factor(&is_variable)) == ERROR) {
            return ERROR;
        }

        if (term_type1 == TPINT && term_type2 != TPINT) {
            return error("The type of the operand must be integer.");
        } else if (term_type1 == TPBOOL && term_type2 != TPBOOL) {
            return error("The type of the operand must be boolean.");
        }

        if (is_variable) {
            /* Load a right value from a variable to calculate */
            assemble_variable_reference_rval(id_referenced_variable);
            is_variable = false;
        }

        if (opr == TSTAR) {
            assemble_MULA();
        } else if (opr == TDIV) {
            assemble_DIVA();
        } else if (opr == TAND) {
            assemble_AND();
        }
    }
    return term_type1;
}

/*!
 * @brief Parsing a factor
 * @param [out] is_variable if token is TNAME, return 1.
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_factor(int *is_variable) {
    int factor_type = TPNONE;
    int exp_type = TPNONE;
    int is_factor_variable = 0;
    int is_expression_variable_only = 0;

    *is_variable = false;

    switch (token) {
        case TNAME:
            if ((factor_type = parse_variable()) == ERROR) {
                return ERROR;
            }
            *is_variable = true;
            break;
        case TNUMBER:
            /* FALLTHROUGH */
        case TFALSE:
            /* FALLTHROUGH */
        case TTRUE:
            /* FALLTHROUGH */
        case TSTRING:
            if ((factor_type = parse_constant()) == ERROR) {
                return ERROR;
            }
            break;
        case TLPAREN:
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if ((factor_type = parse_expression(&is_expression_variable_only)) == ERROR) {
                return ERROR;
            }
            /* (expression) is right value */
            if (is_expression_variable_only) {
                assemble_variable_reference_rval(id_referenced_variable);
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

            if ((factor_type = parse_factor(&is_factor_variable)) == ERROR) {
                return ERROR;
            }
            if (factor_type != TPBOOL) {
                return error("The type of the operand must be boolean.");
            }
            /* TODO: if factor is vaible, variable reference rval */
            break;
        case TINTEGER:
            /* FALLTHROUGH */
        case TBOOLEAN:
            /* FALLTHROUGH */
        case TCHAR:
            if ((factor_type = parse_standard_type()) == ERROR) {
                return ERROR;
            }

            if (token != TLPAREN) {
                return error("Symbol '(' is not found");
            }
            fprintf(stdout, "%s", tokenstr[token]);
            token = scan();

            if ((exp_type = parse_expression(&is_expression_variable_only)) == ERROR) {
                return ERROR;
            }

            if (is_expression_variable_only) {
                assemble_variable_reference_rval(id_referenced_variable);
            }

            if (exp_type & TPARRAY) {
                return error("The type must be a standard type.");
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

    return factor_type;
}

/*!
 * @brief Parsing a constant
 * @return int Returns 0 on success and 1 on failure.
 */
static int parse_constant(void) {
    int constant_type = NORMAL;
    int constant_value;

    switch (token) {
        case TNUMBER:
            fprintf(stdout, "%s", string_attr);
            constant_type = TPINT;
            constant_value = num_attr;
            break;
        case TFALSE:
            /* FALLTHROUGH */
        case TTRUE:
            fprintf(stdout, "%s", tokenstr[token]);
            constant_type = TPBOOL;
            constant_value = (token == TTRUE) ? 1 : 0;
            break;
        case TSTRING:
            if (strlen(string_attr) != 1) {
                return error("Constant string length != 1");
            }
            fprintf(stdout, "'%s'", string_attr);
            constant_type = TPCHAR;
            constant_value = (int)string_attr[0];
            break;
        default:
            return error("Constant is not found.");
    }

    assemble_constant(constant_value);

    token = scan();
    return constant_type;
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
