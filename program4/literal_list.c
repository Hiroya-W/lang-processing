#include "mppl_compiler.h"

/*! Pointers to root */
struct LITERAL *literal_root;

/*!
 * @brief Initialise the list
 */
void init_literal_list() {
    literal_root = NULL;
}

/*!
 * @brief Add a literal
 * @param[in] label Label assigned to a literal
 * @param[in] value Value of literal
 * @return int Return 0 on success and -1 on failure.
 */
int add_literal(char *label, char *value) {
    struct LITERAL *new_literal;

    /* struct LITERAL */
    if ((new_literal = (struct LITERAL *)malloc(sizeof(struct LITERAL))) == NULL) {
        return error("Can not malloc for struct LITERAL in add_literal.\n");
    }
    new_literal->label = label;
    new_literal->value = value;
    new_literal->nextp = literal_root;
    literal_root = new_literal;

    return 0;
}

/*!
 * @brief Release the literal list
 * @param[in] root The root of the list 
 */
void release_literal(void) {
    struct LITERAL *p_literal = literal_root;
    while (p_literal != NULL) {
        struct LITERAL *next_p = p_literal->nextp;
        free(p_literal);
        p_literal = next_p;
    }
    literal_root = NULL;
}

/*!
 * @brief Generating assembly code for all literal
 */
void assemble_literals(void) {
    struct LITERAL *p_literal = literal_root;
    while (p_literal != NULL) {
        fprintf(out_fp, "%s \tDC \t%s\n", p_literal->label, p_literal->value);
        p_literal = p_literal->nextp;
    }
}
