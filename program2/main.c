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

/*! the scanned token */
int token;

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
    if (init_scan(np[1]) < 0) {
        fprintf(stderr, "File %s can not open.\n", np[1]);
        return EXIT_FAILURE;
    }

    token = scan();
    ret = parse_program();

    if (end_scan() < 0) {
        error("function main()");
        fprintf(stderr, "File %s can not close.\n", np[1]);
        return EXIT_FAILURE;
    }
    /* TODO:Output the results. */

    return ret;
}

/*!
 * @brief display an error message
 * @param[in] mes Error message
 * @return Return -1 as an error.
 */
int error(char *mes) {
    fprintf(stderr, "\n ERROR: %s\n", mes);
    return ERROR;
}
