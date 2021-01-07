#include "mppl_compiler.h"

FILE *out_fp;

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

    if ((out_fp = fopen(filename, "w")) == NULL) {
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
