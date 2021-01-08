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
    char *label;
    fprintf(out_fp, "$$%s \tSTART\n", program_name);
    fprintf(out_fp, "\tLAD \tgr0, \t0\n");
    if (create_newlabel(&label) == ERROR) {
        error("function assemble_start()");
        return ERROR;
    }
    fprintf(out_fp, "\tCALL \t%s\n", label);
    fprintf(out_fp, "\tCALL \tFLUSH\n");
    fprintf(out_fp, "\tSVC \t0\n");

    return 0;
}

/*!
 * @brief Create a new label.
 * @param[in] out Pointer to return the label that was created
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

/*!
 * @brief Generating assembly code for if condition
 * @param[in] Label to jump to else
 * @return int Returns 0 on success and -1 on failure.
 */
void assemble_if_condition(char *else_label) {
    fprintf(out_fp, ";if condition\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tCPA \tgr1, \tgr0\n");
    fprintf(out_fp, "\tJZE \t%s\n", else_label);
}

void assemble_else(char *if_end_label, char *else_label) {
    fprintf(out_fp, ";else\n");
    fprintf(out_fp, "\tJUMP \t%s\n", if_end_label);
    fprintf(out_fp, "%s\n", else_label);
}

void assemble_MULA() {
    fprintf(out_fp, ";MULA\n");
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tMULA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}
void assemble_DIVA() {
    fprintf(out_fp, ";DIVA\n");
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tDIVA \tgr1, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}
void assemble_AND() {
    fprintf(out_fp, ";DIVA\n");
    fprintf(out_fp, "\tPOP \tgr2\n");
    fprintf(out_fp, "\tPOP \tgr1\n");
    fprintf(out_fp, "\tAND \tgr1, \tgr2\n");
    fprintf(out_fp, "\tPUSH \t0, \tgr1\n");
}
