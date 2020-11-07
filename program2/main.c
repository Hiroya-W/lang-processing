#include <stdio.h>

#include "pretty-printer.h"

/*! list of keywords */
struct KEY key[KEYWORDSIZE] = {
    {"and", TAND},
    {"array", TARRAY},
    {"begin", TBEGIN},
    {"boolean", TBOOLEAN},
    {"break", TBREAK},
    {"call", TCALL},
    {"char", TCHAR},
    {"div", TDIV},
    {"do", TDO},
    {"else", TELSE},
    {"end", TEND},
    {"false", TFALSE},
    {"if", TIF},
    {"integer", TINTEGER},
    {"not", TNOT},
    {"of", TOF},
    {"or", TOR},
    {"procedure", TPROCEDURE},
    {"program", TPROGRAM},
    {"read", TREAD},
    {"readln", TREADLN},
    {"return", TRETURN},
    {"then", TTHEN},
    {"true", TTRUE},
    {"var", TVAR},
    {"while", TWHILE},
    {"write", TWRITE},
    {"writeln", TWRITELN}};

/*! the scanned token */
int token;
static char *file_name;

/*!
 * @brief main function
 * @param[in] nc The number of arguments
 * @param[in] np File name to read
 * @return int Returns 0 on success and 1 on failure.
 */
int main(int nc, char *np[]) {
    int ret;
    if (nc < 2) {
        error("function main()");
        fprintf(stderr, "File name id not given.\n");
        return EXIT_FAILURE;
    }

    file_name = np[1];

    if (init_scan(file_name) < 0) {
        fprintf(stderr, "File %s can not open.\n", file_name);
        return EXIT_FAILURE;
    }

    token = scan();
    ret = parse_program();

    if (end_scan() < 0) {
        error("function main()");
        fprintf(stderr, "File %s can not close.\n", file_name);
        return EXIT_FAILURE;
    }

    fflush(stdout);
    return ret;
}

/*!
 * @brief display an error message
 * @param[in] mes Error message
 * @return Return -1 as an error.
 */
int error(char *mes) {
    fprintf(stdout, "\n");
    fprintf(stderr, "--- %s:%d\n", file_name, get_linenum());
    fprintf(stderr, " |- ERROR: %s\n", mes);
    return ERROR;
}
