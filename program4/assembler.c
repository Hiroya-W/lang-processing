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
 * @brief Create a new label.
 * @param[in] out Pointer to return the label that was created
 * @return int Returns 0 on success and -1 on failure.
 */
int create_newlabel(char **out) {
    char *newlabel;

    /* struct ID */
    if ((newlabel = (char *)malloc(sizeof(char) * (LABEL_SIZE + 1))) == NULL) {
        return error("can not malloc in create_newlabel\n");
    }

    label_counter++;

    sprintf(newlabel, "L%04d", label_counter);
    *out = newlabel;

    return 0;
}

void assemble_if_condition(char *else_label) {
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
    fprintf(out_fp, "\tJZE \t%s\n", else_label);
}
