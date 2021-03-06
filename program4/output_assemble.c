#include "mppl_compiler.h"

/*! maximum length of a label */
#define LABEL_SIZE 128

/*! File pointer of the output file */
FILE *out_fp;
/*! Count the number of labels created */
int label_counter = 0;

/*!
 * @brief Initialize the output file
 * @param[in] filename MPPL source file name
 * @return int Returns 0 on success and -1 on failure.
 */
int init_assemble(char *filename_mppl) {
    char *filename = strtok(filename_mppl, ".");
    char filename_csl[128];
    /* hoge.mpl -> hoge.csl */
    sprintf(filename_csl, "%s.csl", filename);

    if ((out_fp = fopen(filename_csl, "w")) == NULL) {
        error("fopen() returns NULL");
        error("function init_assemble()");
        return -1;
    }

    return 0;
}

/*!
 * @brief Close the output file
 * @return int Returns 0 on success and -1 on failure.
 */
int end_assemble(void) {
    if (fclose(out_fp) == EOF) {
        error("function end_assemble()");
        fprintf(stderr, "fclose() returns EOF.");
        return -1;
    }
    return 0;
}

/*!
 * @brief Generating assembly code for START
 * @param[in] Program name
 * @return int Returns 0 on success and -1 on failure.
 */
int assemble_start(char *program_name) {
    fprintf(out_fp, "$$%s \tSTART\n", program_name);
    fprintf(out_fp, "\tLAD \tgr0, \t0\n");
    fprintf(out_fp, "\tCALL \tL0001\n");
    fprintf(out_fp, "\tCALL \tFLUSH\n");
    fprintf(out_fp, "\tSVC \t0\n");

    return 0;
}

/*!
 * @brief Create a new label.
 * @param[out] out Pointer to return the label that was created
 * @return int Returns 0 on success and -1 on failure.
 */
int create_newlabel(char **out) {
    char *new_label;

    /* char for newlabel */
    if ((new_label = (char *)malloc(sizeof(char) * (LABEL_SIZE + 1))) == NULL) {
        return error("can not malloc in create_newlabel\n");
    }

    label_counter++;

    sprintf(new_label, "L%04d", label_counter);
    *out = new_label;

    return 0;
}

void assemble_block_end(void) {
    fprintf(out_fp, "\tRET\n");
}

/*!
 * @brief Generating assembly code for procedure definition
 */
void assemble_procedure_definition(void) {
    /* fprintf(out_fp, ";procedure declaration\n"); */
    fprintf(out_fp, "$%s\n", current_procedure_name);
}

/*!
 * @brief Generating assembly code for beginning of procedure statement
 * Process the parameters and the address to be returned when the function terminates
 */
int assemble_procedure_begin(void) {
    struct ID *p_id;
    struct ID *p_id_list = NULL;

    /* Reverse the order of the parameters. */
    p_id = localidroot;
    if (p_id != NULL) {
        while (p_id != NULL && p_id->ispara == 1) {
            struct ID *p_id_temp = NULL;
            /* struct ID */
            if ((p_id_temp = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
                return error("can not malloc1 for struct ID in id_register_to_tab\n");
            }
            p_id_temp->name = p_id->name;
            p_id_temp->nextp = p_id_list;
            p_id_list = p_id_temp;
            p_id = p_id->nextp;
        }
    } else {
        /* If the procedure does not have parameters, it does nothing.*/
        return 0;
    }

    fprintf(out_fp, "\tPOP\t gr2\n"); /* gr2: return pointer */
    /* Set a value to parameters */
    p_id = p_id_list;
    while (p_id != NULL) {
        fprintf(out_fp, "\tPOP gr1\n");
        fprintf(out_fp, "\tST \tgr1, \t$%s%c%s\n", p_id->name, '%', current_procedure_name);
        p_id = p_id->nextp;
    }

    /* push a return pointer */
    fprintf(out_fp, "\tPUSH \t0, \tgr2\n");
    return 0;
}

/*!
 * @brief Generating assembly code for end of procedure statement
 */
void assemble_procedure_end() {
    fprintf(out_fp, "\tRET\n");
}

/*!
 * @brief Generating assembly code for variable declaration
 * @param[in] else_label Label to jump to else
 */
void assemble_variable_declaration(char *variable_name, char *procname, struct TYPE **type) {
    /* fprintf(out_fp, ";variable declaration\n"); */
    fprintf(out_fp, "$%s", variable_name);
    if (procname != NULL) {
        fprintf(out_fp, "%c%s", '%', procname);
    }
    if ((*type)->ttype & TPARRAY) {
        fprintf(out_fp, " \tDS \t%d\n", (*type)->arraysize);
    } else {
        fprintf(out_fp, " \tDC \t0\n");
    }
}

/*!
 * @brief Generating assembly code for left value of variable 
 * @param[in] referenced_variable referenced variable
 */
void assemble_variable_reference_lval(struct ID *referenced_variable) {
    if (referenced_variable->ispara) {
        /* if id is parameter, id has procname */
        fprintf(out_fp, "\tLD \tgr1, \t$%s%%%s\n", referenced_variable->name, referenced_variable->procname);
    } else if (referenced_variable->procname != NULL) {
        fprintf(out_fp, "\tLAD \tgr1, \t$%s%%%s\n", referenced_variable->name, referenced_variable->procname);
    } else {
        fprintf(out_fp, "\tLAD \tgr1, \t$%s\n", referenced_variable->name);
    }

    if (referenced_variable->itp->ttype & TPARRAY) {
        /* gr1 is head */
        fprintf(out_fp, "\tPOP \tgr2\n"); /* gr2 is index */                             /* Check for out-of-array references */
        fprintf(out_fp, "\tLAD \tgr3, \t%d\n", referenced_variable->itp->arraysize - 1); /* gr3 is max index */
        fprintf(out_fp, "\tCPA \tgr2, \tgr3\n");                                         /* gr2 - gr3 */
        fprintf(out_fp, "\tJPL \tEROV\n");                                               /* if gr2 - gr3 is positive, it is an out-of-array reference */
        fprintf(out_fp, "\tADDA \tgr1, \tgr2\n");                                        /* gr1 <- address(gr1(head) + gr2(index)) */
        fprintf(out_fp, "\tJOV \tEOVF\n");
    }

    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for right value of variable 
 * @param[in] referenced_variable referenced variable
 */
void assemble_variable_reference_rval(struct ID *referenced_variable) {
    if (referenced_variable->itp->ttype & TPARRAY) {
        assemble_variable_reference_lval(referenced_variable); /* get address */
        fprintf(out_fp, "\tPOP \tgr1\n");
        fprintf(out_fp, "\tLD \tgr1, \t0, \tgr1\n"); /* get rval from address */
    } else {
        if (referenced_variable->ispara) {
            /* if id is parameter, id has procname */
            fprintf(out_fp, "\tLD \tgr1, \t$%s%%%s\n", referenced_variable->name, referenced_variable->procname);
            fprintf(out_fp, "\tLD \tgr1, \t0, \tgr1\n");
        } else if (referenced_variable->procname != NULL) {
            fprintf(out_fp, "\tLD \tgr1, \t$%s%%%s\n", referenced_variable->name, referenced_variable->procname);
        } else {
            fprintf(out_fp, "\tLD \tgr1, \t$%s\n", referenced_variable->name);
        }
    }

    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for to assign real parameters to address 
 * If the real parameter does not have a left value, assign an address
 */
void assemble_assign_real_param_to_address(void) {
    char *label = NULL;
    create_newlabel(&label);
    add_literal(&literal_root, label, "0");
    fprintf(out_fp, "\tLAD \tgr2, \t%s\n", label);
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tST \tgr1, \t0, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr2\n");
}

/*!
 * @brief Generating assembly code for assignment statement
 */
void assemble_assign(void) {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tST \tgr2, \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for if condition
 * @param[in] else_label Label to jump to else
 */
void assemble_if_condition(char *else_label) {
    /* fprintf(out_fp, ";if condition\n"); */
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
    fprintf(out_fp, "\tJZE \t%s\n", else_label);
}

/*!
 * @brief Generating assembly code for else section
 * @param[in] if_end_label Label to jump to end of condition_statement
 * @param[in] else_label Label to start of else section
 */
void assemble_else(char *if_end_label, char *else_label) {
    /* fprintf(out_fp, ";else\n"); */
    fprintf(out_fp, "\tJUMP \t%s\n", if_end_label);
    fprintf(out_fp, "%s\n", else_label);
}

/*!
 * @brief Generating assembly code for condition of iteration statement
 */
void assemble_iteration_condition(char *bottom_label) {
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
    fprintf(out_fp, "\tJZE \t%s\n", bottom_label);
}

/*!
 * @brief Generating assembly code for break 
 */
void assemble_break(void) {
    fprintf(out_fp, "\tJUMP \t%s\n", while_end_literal_root->label);
}

/*!
 * @brief Generating assembly code for return
 */
void assemble_return(void) {
    if (in_subprogram_declaration) {
        fprintf(out_fp, "\tRET\n");
    } else {
        fprintf(out_fp, "\tSVC \t0\n");
    }
}

/*!
 * @brief Generating assembly code for call statemnt
 */
void assemble_call(struct ID *id_procedure) {
    fprintf(out_fp, "\tCALL $%s\n", id_procedure->name);
}

/*!
 * @brief Generating assembly code for expression
 */
void assemble_expression(int relational_operator_token) {
    char *jmp_true_label = NULL;
    char *jmp_false_label = NULL;
    create_newlabel(&jmp_true_label);
    create_newlabel(&jmp_false_label);

    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tCPA \tgr1, \tgr2\n");

    switch (relational_operator_token) {
        case TEQUAL: /* = */
            fprintf(out_fp, "\tJZE \t%s\n", jmp_true_label);
            break;
        case TNOTEQ: /* <> */
            fprintf(out_fp, "\tJNZ \t%s\n", jmp_true_label);
            break;
        case TLE: /* < */
            fprintf(out_fp, "\tJMI \t%s\n", jmp_true_label);
            break;
        case TLEEQ: /* <= */
            fprintf(out_fp, "\tJMI \t%s\n", jmp_true_label);
            fprintf(out_fp, "\tJZE \t%s\n", jmp_true_label);
            break;
        case TGR: /* > */
            fprintf(out_fp, "\tJPL \t%s\n", jmp_true_label);
            break;
        case TGREQ: /* >= */
            fprintf(out_fp, "\tJPL \t%s\n", jmp_true_label);
            fprintf(out_fp, "\tJZE \t%s\n", jmp_true_label);
            break;
    }

    fprintf(out_fp, "\tLD \tgr1, \tgr0\n"); /* return 0 */
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    fprintf(out_fp, "\tJUMP \t%s\n", jmp_false_label);

    fprintf(out_fp, "%s\n", jmp_true_label);
    fprintf(out_fp, "\tLAD \tgr1, \t1\n"); /* return 1 */
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    fprintf(out_fp, "%s\n", jmp_false_label);
}

/*!
 * @brief Generating assembly code for multiply the negatives
 */
void assemble_minus_sign() {
    fprintf(out_fp, "\tLAD \tgr1, \t-1\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    assemble_MULA();
}

/*!
 * @brief Generating assembly code for ADDA
 */
void assemble_ADDA() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tADDA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tJOV \tEOVF\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for SUBA
 */
void assemble_SUBA() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tSUBA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tJOV \tEOVF\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for OR
 */
void assemble_OR() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tOR \tgr1, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for constant
 * @param[in] param right value of constant
 */
int assemble_constant(int value) {
    fprintf(out_fp, "\tLAD \tgr1, \t%d\n", value);
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    return 0;
}

/*!
 * @brief Generating assembly code for NOT operation
 */
void assemble_not_factor(void) {
    char *jmp_zero_label = NULL;
    char *jmp_not_end_label = NULL;
    create_newlabel(&jmp_zero_label);
    create_newlabel(&jmp_not_end_label);

    fprintf(out_fp, "\tPOP \tgr1\n"); /* factor value */
    fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
    fprintf(out_fp, "\tJNZ \t%s\n", jmp_zero_label); /* expression value != 0 ? 0(false) : 1(true) */
    fprintf(out_fp, "\tLAD \tgr1, 1\n");             /* return 1 */
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    fprintf(out_fp, "\tJUMP \t%s\n", jmp_not_end_label);

    fprintf(out_fp, "%s\n", jmp_zero_label);
    fprintf(out_fp, "\tLD \tgr1, \tgr0\n"); /* return 0 */
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
    fprintf(out_fp, "%s\n", jmp_not_end_label);
}

/*!
 * @brief Generating assembly code for type conversion
 */
void assemble_cast(int to_type, int from_type) {
    if (from_type == TPINT) {
        if (to_type == TPINT) {
            /* no operation */
        } else if (to_type == TPBOOL) {
            char *jmp_true_label = NULL;
            char *jmp_cast_end_label = NULL;
            create_newlabel(&jmp_true_label);
            create_newlabel(&jmp_cast_end_label);

            fprintf(out_fp, "\tPOP \tgr1\n"); /* expression value */
            fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
            fprintf(out_fp, "\tJNZ \t%s\n", jmp_true_label); /* expression value != 0 ? 1(true) : 0(false) */
            fprintf(out_fp, "\tLD \tgr1, \tgr0\n");          /* return 0 */
            fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
            fprintf(out_fp, "\tJUMP \t%s\n", jmp_cast_end_label);

            fprintf(out_fp, "%s\n", jmp_true_label);
            fprintf(out_fp, "\tLAD \tgr1, \t1\n"); /* return 1 */
            fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
            fprintf(out_fp, "%s\n", jmp_cast_end_label);
        } else if (to_type == TPCHAR) {
            fprintf(out_fp, "\tPOP \tgr1\n"); /* expression value */
            fprintf(out_fp, "\tLAD \tgr2, \t#007F\n");
            fprintf(out_fp, "\tAND \tgr1, \tgr2\n");
            fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
        }
    } else if (from_type == TPBOOL) {
        /* no operation */
    } else if (from_type == TPCHAR) {
        if (to_type == TPBOOL) {
            char *jmp_true_label = NULL;
            char *jmp_cast_end_label = NULL;
            create_newlabel(&jmp_true_label);
            create_newlabel(&jmp_cast_end_label);

            fprintf(out_fp, "\tPOP \tgr1\n"); /* expression value */
            fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
            fprintf(out_fp, "\tJNZ \t%s\n", jmp_true_label); /* expression value != 0 ? 1(true) : 0(false) */
            fprintf(out_fp, "\tLD \tgr1, \tgr0\n");          /* return 0 */
            fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
            fprintf(out_fp, "\tJUMP \t%s\n", jmp_cast_end_label);

            fprintf(out_fp, "%s\n", jmp_true_label);
            fprintf(out_fp, "\tLAD \tgr1, \t1\n"); /* return 1 */
            fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
            fprintf(out_fp, "%s\n", jmp_cast_end_label);
        } else if (to_type == TPINT || to_type == TPCHAR) {
            /* no operation */
        }
    }
}

/*!
 * @brief Generating assembly code for product operation
 */
void assemble_MULA() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tMULA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tJOV \tEOVF\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for division operation
 */
void assemble_DIVA() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tDIVA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tJOV \tE0DIV\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for AND operation
 */
void assemble_AND() {
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tAND \tgr1, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}

/*!
 * @brief Generating assembly code for output strings
 * @param [in] strings the output strings 
 * @return int Returns 0 on success and -1 on failure.
 */
int assemble_output_format_string(char *strings) {
    char *label;
    char *surrounded_strings;

    if ((surrounded_strings = (char *)malloc(sizeof(char) * strlen(strings) + 2)) == NULL) {
        return error("Can not malloc for char in assemble_output_format_string.\n");
    }
    create_newlabel(&label);
    sprintf(surrounded_strings, "'%s'", strings);
    add_literal(&literal_root, label, surrounded_strings);

    fprintf(out_fp, "\tLAD \tgr1, %s\n", label);
    fprintf(out_fp, "\tLD \tgr2, \tgr0\n");
    fprintf(out_fp, "\tCALL \tWRITESTR\n");
    return 0;
}

/*!
 * @brief Generating assembly code for output standard type
 * @param [in] type Type of the output content
 * @param [in] num Number of digits to display the content
 */
void assemble_output_format_standard_type(int type, int num) {
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tLAD \tgr2, \t%d\n", num);

    switch (type) {
        case TPINT:
            fprintf(out_fp, "\tCALL \tWRITEINT\n");
            break;
        case TPCHAR:
            fprintf(out_fp, "\tCALL \tWRITECHAR\n");
            break;
        case TPBOOL:
            fprintf(out_fp, "\tCALL \tWRITEBOOL\n");
            break;
    }
}

/*!
 * @brief Generating assembly code for newline
 */
void assemble_output_line() {
    fprintf(out_fp, "\tCALL \tWRITELINE\n");
}

/*!
 * @brief Generating assembly code read statemnt
 */
void assemble_read(int type) {
    fprintf(out_fp, "\tPOP \tgr1\n");
    switch (type) {
        case TPINT:
            fprintf(out_fp, "\tCALL \tREADINT\n");
            break;
        case TPCHAR:
            fprintf(out_fp, "\tCALL \tREADCHAR\n");
            break;
    }
}

/*!
 * @brief Generating assembly code read with new line
 */
void assemble_read_line() {
    fprintf(out_fp, "\tCALL \tREADLINE\n");
}

/*!
 * @brief Generating library
 */
void assemble_library() {
    fprintf(out_fp, ";-- Library --\n");
    fprintf(out_fp, "EOVF\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  LAD  gr1, EOVF1\n");
    fprintf(out_fp, "  LD  gr2, gr0\n");
    fprintf(out_fp, "  CALL  WRITESTR\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  SVC  1  ;  overflow error stop\n");
    fprintf(out_fp, "EOVF1    DC  '***** Run-Time Error : Overflow *****'\n");
    fprintf(out_fp, "E0DIV\n");
    fprintf(out_fp, "  JNZ  EOVF\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  LAD  gr1, E0DIV1\n");
    fprintf(out_fp, "  LD  gr2, gr0\n");
    fprintf(out_fp, "  CALL  WRITESTR\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  SVC  2  ;  0-divide error stop\n");
    fprintf(out_fp, "E0DIV1    DC  '***** Run-Time Error : Zero-Divide *****'\n");
    fprintf(out_fp, "EROV\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  LAD  gr1, EROV1\n");
    fprintf(out_fp, "  LD  gr2, gr0\n");
    fprintf(out_fp, "  CALL  WRITESTR\n");
    fprintf(out_fp, "  CALL  WRITELINE\n");
    fprintf(out_fp, "  SVC  3  ;  range-over error stop\n");
    fprintf(out_fp, "EROV1    DC  '***** Run-Time Error : Range-Over in Array Index *****'\n");
    fprintf(out_fp, "WRITECHAR\n");
    fprintf(out_fp, "; gr1の値（文字）をgr2のけた数で出力する．\n");
    fprintf(out_fp, "; gr2が0なら必要最小限の桁数で出力する\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD  gr6, SPACE\n");
    fprintf(out_fp, "  LD  gr7, OBUFSIZE\n");
    fprintf(out_fp, "WC1\n");
    fprintf(out_fp, "  SUBA  gr2, ONE  ; while(--c > 0) {\n");
    fprintf(out_fp, "  JZE  WC2\n");
    fprintf(out_fp, "  JMI  WC2\n");
    fprintf(out_fp, "  ST  gr6, OBUF,gr7  ;  *p++ = ' ';\n");
    fprintf(out_fp, "  CALL  BOVFCHECK\n");
    fprintf(out_fp, "  JUMP  WC1  ; }\n");
    fprintf(out_fp, "WC2\n");
    fprintf(out_fp, "  ST  gr1, OBUF,gr7  ; *p++ = gr1;\n");
    fprintf(out_fp, "  CALL  BOVFCHECK\n");
    fprintf(out_fp, "  ST  gr7, OBUFSIZE\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "WRITESTR\n");
    fprintf(out_fp, "; gr1が指す文字列をgr2のけた数で出力する．\n");
    fprintf(out_fp, "; gr2が0なら必要最小限の桁数で出力する\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD  gr6, gr1  ; p = gr1;\n");
    fprintf(out_fp, "WS1\n");
    fprintf(out_fp, "  LD  gr4, 0,gr6  ; while(*p != '\\0') {\n");
    fprintf(out_fp, "  JZE  WS2\n");
    fprintf(out_fp, "  ADDA  gr6, ONE  ;  p++;\n");
    fprintf(out_fp, "  SUBA  gr2, ONE  ;  c--;\n");
    fprintf(out_fp, "  JUMP  WS1  ; }\n");
    fprintf(out_fp, "WS2\n");
    fprintf(out_fp, "  LD  gr7, OBUFSIZE  ; q = OBUFSIZE;\n");
    fprintf(out_fp, "  LD  gr5, SPACE\n");
    fprintf(out_fp, "WS3\n");
    fprintf(out_fp, "  SUBA  gr2, ONE  ; while(--c >= 0) {\n");
    fprintf(out_fp, "  JMI  WS4\n");
    fprintf(out_fp, "  ST  gr5, OBUF,gr7  ;  *q++ = ' ';\n");
    fprintf(out_fp, "  CALL  BOVFCHECK\n");
    fprintf(out_fp, "  JUMP  WS3  ; }\n");
    fprintf(out_fp, "WS4\n");
    fprintf(out_fp, "  LD  gr4, 0,gr1  ; while(*gr1 != '\\0') {\n");
    fprintf(out_fp, "  JZE  WS5\n");
    fprintf(out_fp, "  ST  gr4, OBUF,gr7  ;  *q++ = *gr1++;\n");
    fprintf(out_fp, "  ADDA  gr1, ONE\n");
    fprintf(out_fp, "  CALL  BOVFCHECK\n");
    fprintf(out_fp, "  JUMP  WS4  ; }\n");
    fprintf(out_fp, "WS5\n");
    fprintf(out_fp, "  ST  gr7, OBUFSIZE  ; OBUFSIZE = q;\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "BOVFCHECK\n");
    fprintf(out_fp, "    ADDA  gr7, ONE\n");
    fprintf(out_fp, "    CPA   gr7, BOVFLEVEL\n");
    fprintf(out_fp, "    JMI  BOVF1\n");
    fprintf(out_fp, "    CALL  WRITELINE\n");
    fprintf(out_fp, "    LD gr7, OBUFSIZE\n");
    fprintf(out_fp, "BOVF1\n");
    fprintf(out_fp, "    RET\n");
    fprintf(out_fp, "BOVFLEVEL  DC 256\n");
    fprintf(out_fp, "WRITEINT\n");
    fprintf(out_fp, "; gr1の値（整数）をgr2のけた数で出力する．\n");
    fprintf(out_fp, "; gr2が0なら必要最小限の桁数で出力する\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD  gr7, gr0  ; flag = 0;\n");
    fprintf(out_fp, "  CPA  gr1, gr0  ; if(gr1>=0) goto WI1;\n");
    fprintf(out_fp, "  JPL  WI1\n");
    fprintf(out_fp, "  JZE  WI1\n");
    fprintf(out_fp, "  LD  gr4, gr0  ; gr1= - gr1;\n");
    fprintf(out_fp, "  SUBA  gr4, gr1\n");
    fprintf(out_fp, "  CPA  gr4, gr1\n");
    fprintf(out_fp, "  JZE  WI6\n");
    fprintf(out_fp, "  LD  gr1, gr4\n");
    fprintf(out_fp, "  LD  gr7, ONE  ; flag = 1;\n");
    fprintf(out_fp, "WI1\n");
    fprintf(out_fp, "  LD  gr6, SIX  ; p = INTBUF+6;\n");
    fprintf(out_fp, "  ST  gr0, INTBUF,gr6  ; *p = '\\0';\n");
    fprintf(out_fp, "  SUBA  gr6, ONE  ; p--;\n");
    fprintf(out_fp, "  CPA  gr1, gr0  ; if(gr1 == 0)\n");
    fprintf(out_fp, "  JNZ  WI2\n");
    fprintf(out_fp, "  LD  gr4, ZERO  ;  *p = '0';\n");
    fprintf(out_fp, "  ST  gr4, INTBUF,gr6\n");
    fprintf(out_fp, "  JUMP  WI5  ; }\n");
    fprintf(out_fp, "WI2      ; else {\n");
    fprintf(out_fp, "  CPA  gr1, gr0  ;  while(gr1 != 0) {\n");
    fprintf(out_fp, "  JZE  WI3\n");
    fprintf(out_fp, "  LD  gr5, gr1  ;   gr5 = gr1 - (gr1 / 10) * 10;\n");
    fprintf(out_fp, "  DIVA  gr1, TEN  ;   gr1 /= 10;\n");
    fprintf(out_fp, "  LD  gr4, gr1\n");
    fprintf(out_fp, "  MULA  gr4, TEN\n");
    fprintf(out_fp, "  SUBA  gr5, gr4\n");
    fprintf(out_fp, "  ADDA  gr5, ZERO  ;   gr5 += '0';\n");
    fprintf(out_fp, "  ST  gr5, INTBUF,gr6  ;   *p = gr5;\n");
    fprintf(out_fp, "  SUBA  gr6, ONE  ;   p--;\n");
    fprintf(out_fp, "  JUMP  WI2  ;  }\n");
    fprintf(out_fp, "WI3\n");
    fprintf(out_fp, "  CPA  gr7, gr0  ;  if(flag != 0) {\n");
    fprintf(out_fp, "  JZE  WI4\n");
    fprintf(out_fp, "  LD  gr4, MINUS  ;   *p = '-';\n");
    fprintf(out_fp, "  ST  gr4, INTBUF,gr6\n");
    fprintf(out_fp, "  JUMP  WI5  ;  }\n");
    fprintf(out_fp, "WI4\n");
    fprintf(out_fp, "  ADDA  gr6, ONE  ;  else p++;\n");
    fprintf(out_fp, "    ; }\n");
    fprintf(out_fp, "WI5\n");
    fprintf(out_fp, "  LAD  gr1, INTBUF,gr6  ; gr1 = p;\n");
    fprintf(out_fp, "  CALL  WRITESTR  ; WRITESTR();\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "WI6\n");
    fprintf(out_fp, "  LAD  gr1, MMINT\n");
    fprintf(out_fp, "  CALL  WRITESTR  ; WRITESTR();\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "MMINT    DC  '-32768'\n");
    fprintf(out_fp, "WRITEBOOL\n");
    fprintf(out_fp, "; gr1の値（真理値）が0なら'FALSE'を\n");
    fprintf(out_fp, "; 0以外なら'TRUE'をgr2のけた数で出力する．\n");
    fprintf(out_fp, "; gr2が0なら必要最小限の桁数で出力する\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  CPA  gr1, gr0  ; if(gr1 != 0)\n");
    fprintf(out_fp, "  JZE  WB1\n");
    fprintf(out_fp, "  LAD  gr1, WBTRUE  ;  gr1 = \" TRUE \";\n");
    fprintf(out_fp, "  JUMP  WB2\n");
    fprintf(out_fp, "WB1      ; else\n");
    fprintf(out_fp, "  LAD  gr1, WBFALSE  ;  gr1 = \" FALSE \";\n");
    fprintf(out_fp, "WB2\n");
    fprintf(out_fp, "  CALL  WRITESTR  ; WRITESTR();\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "WBTRUE    DC  'TRUE'\n");
    fprintf(out_fp, "WBFALSE    DC  'FALSE'\n");
    fprintf(out_fp, "WRITELINE\n");
    fprintf(out_fp, "; 改行を出力する\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD  gr7, OBUFSIZE\n");
    fprintf(out_fp, "  LD  gr6, NEWLINE\n");
    fprintf(out_fp, "  ST  gr6, OBUF,gr7\n");
    fprintf(out_fp, "  ADDA  gr7, ONE\n");
    fprintf(out_fp, "  ST  gr7, OBUFSIZE\n");
    fprintf(out_fp, "  OUT  OBUF, OBUFSIZE\n");
    fprintf(out_fp, "  ST  gr0, OBUFSIZE\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "FLUSH\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD gr7, OBUFSIZE\n");
    fprintf(out_fp, "  JZE FL1\n");
    fprintf(out_fp, "  CALL WRITELINE\n");
    fprintf(out_fp, "FL1\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "READCHAR\n");
    fprintf(out_fp, "; gr1が指す番地に文字一つを読み込む\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "  LD  gr5, RPBBUF  ; if(RPBBUF != '\\0') {\n");
    fprintf(out_fp, "  JZE  RC0\n");
    fprintf(out_fp, "  ST  gr5, 0,gr1  ;  *gr1 = RPBBUF;\n");
    fprintf(out_fp, "  ST  gr0, RPBBUF  ;  RPBBUF = '\\0'\n");
    fprintf(out_fp, "  JUMP  RC3  ;  return; }\n");
    fprintf(out_fp, "RC0\n");
    fprintf(out_fp, "  LD  gr7, INP  ; inp = INP;\n");
    fprintf(out_fp, "  LD  gr6, IBUFSIZE  ; if(IBUFSIZE == 0) {\n");
    fprintf(out_fp, "  JNZ  RC1\n");
    fprintf(out_fp, "  IN  IBUF, IBUFSIZE  ;  IN();\n");
    fprintf(out_fp, "  LD  gr7, gr0  ;  inp = 0;\n");
    fprintf(out_fp, "    ; }\n");
    fprintf(out_fp, "RC1\n");
    fprintf(out_fp, "  CPA  gr7, IBUFSIZE  ; if(inp == IBUFSIZE) {\n");
    fprintf(out_fp, "  JNZ  RC2\n");
    fprintf(out_fp, "  LD  gr5, NEWLINE  ;  *gr1 = '\\n';\n");
    fprintf(out_fp, "  ST  gr5, 0,gr1\n");
    fprintf(out_fp, "  ST  gr0, IBUFSIZE  ;  IBUFSIZE = INP = 0;\n");
    fprintf(out_fp, "  ST  gr0, INP\n");
    fprintf(out_fp, "  JUMP  RC3  ; }\n");
    fprintf(out_fp, "RC2      ; else {\n");
    fprintf(out_fp, "  LD  gr5, IBUF,gr7  ;  *gr1 = *inp++;\n");
    fprintf(out_fp, "  ADDA  gr7, ONE\n");
    fprintf(out_fp, "  ST  gr5, 0,gr1\n");
    fprintf(out_fp, "  ST  gr7, INP  ;  INP = inp;\n");
    fprintf(out_fp, "RC3      ; }\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "READINT\n");
    fprintf(out_fp, ";gr1が指す番地に整数値一つを読み込む\n");
    fprintf(out_fp, "  RPUSH\n");
    fprintf(out_fp, "RI1      ; do {\n");
    fprintf(out_fp, "  CALL  READCHAR  ;  ch = READCHAR();\n");
    fprintf(out_fp, "  LD  gr7, 0,gr1\n");
    fprintf(out_fp, "  CPA  gr7, SPACE  ; } while(ch == ' ' || ch == '\\t' || ch == '\\n');\n");
    fprintf(out_fp, "  JZE  RI1\n");
    fprintf(out_fp, "  CPA  gr7, TAB\n");
    fprintf(out_fp, "  JZE  RI1\n");
    fprintf(out_fp, "  CPA  gr7, NEWLINE\n");
    fprintf(out_fp, "  JZE  RI1\n");
    fprintf(out_fp, "  LD  gr5, ONE  ; flag = 1\n");
    fprintf(out_fp, "  CPA  gr7, MINUS  ; if(ch == '-') {\n");
    fprintf(out_fp, "  JNZ  RI4\n");
    fprintf(out_fp, "  LD  gr5, gr0  ;  flag = 0;\n");
    fprintf(out_fp, "  CALL  READCHAR  ;  ch = READCHAR();\n");
    fprintf(out_fp, "  LD  gr7, 0,gr1\n");
    fprintf(out_fp, "RI4      ; }\n");
    fprintf(out_fp, "  LD  gr6, gr0  ; v = 0;\n");
    fprintf(out_fp, "RI2\n");
    fprintf(out_fp, "  CPA  gr7, ZERO  ; while('0' <= ch && ch <= '9') {\n");
    fprintf(out_fp, "  JMI  RI3\n");
    fprintf(out_fp, "  CPA  gr7, NINE\n");
    fprintf(out_fp, "  JPL  RI3\n");
    fprintf(out_fp, "  MULA  gr6, TEN  ;  v = v*10+ch-'0';\n");
    fprintf(out_fp, "  ADDA  gr6, gr7\n");
    fprintf(out_fp, "  SUBA  gr6, ZERO\n");
    fprintf(out_fp, "  CALL  READCHAR  ;  ch = READSCHAR();\n");
    fprintf(out_fp, "  LD  gr7, 0,gr1\n");
    fprintf(out_fp, "  JUMP  RI2  ; }\n");
    fprintf(out_fp, "RI3\n");
    fprintf(out_fp, "  ST  gr7, RPBBUF  ; ReadPushBack();\n");
    fprintf(out_fp, "  ST  gr6, 0,gr1  ; *gr1 = v;\n");
    fprintf(out_fp, "  CPA  gr5, gr0  ; if(flag == 0) {\n");
    fprintf(out_fp, "  JNZ  RI5\n");
    fprintf(out_fp, "  SUBA  gr5, gr6  ;  *gr1 = -v;\n");
    fprintf(out_fp, "  ST  gr5, 0,gr1\n");
    fprintf(out_fp, "RI5      ; }\n");
    fprintf(out_fp, "  RPOP\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "READLINE\n");
    fprintf(out_fp, "; 入力を改行コードまで（改行コードも含む）読み飛ばす\n");
    fprintf(out_fp, "  ST  gr0, IBUFSIZE\n");
    fprintf(out_fp, "  ST  gr0, INP\n");
    fprintf(out_fp, "  ST  gr0, RPBBUF\n");
    fprintf(out_fp, "  RET\n");
    fprintf(out_fp, "ONE    DC  1\n");
    fprintf(out_fp, "SIX    DC  6\n");
    fprintf(out_fp, "TEN    DC  10\n");
    fprintf(out_fp, "SPACE    DC  #0020  ; ' '\n");
    fprintf(out_fp, "MINUS    DC  #002D  ; '-'\n");
    fprintf(out_fp, "TAB    DC  #0009  ; '\\t'\n");
    fprintf(out_fp, "ZERO    DC  #0030  ; '0'\n");
    fprintf(out_fp, "NINE    DC  #0039  ; '9'\n");
    fprintf(out_fp, "NEWLINE    DC  #000A  ; '\\n'\n");
    fprintf(out_fp, "INTBUF    DS  8\n");
    fprintf(out_fp, "OBUFSIZE  DC  0\n");
    fprintf(out_fp, "IBUFSIZE  DC  0\n");
    fprintf(out_fp, "INP    DC  0\n");
    fprintf(out_fp, "OBUF    DS  257\n");
    fprintf(out_fp, "IBUF    DS  257\n");
    fprintf(out_fp, "RPBBUF    DC  0\n");
    fprintf(out_fp, "	END\n");
}
