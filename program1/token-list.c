#include "token-list.h"

#include <stdio.h>
#include <stdlib.h>

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

/*! Token counter  */
int numtoken[NUMOFTOKEN + 1];

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

/*!
 * @brief main function
 * @param[in] nc The number of arguments
 * @param[in] np File name to read
 * @return int Returns 0 on success and 1 on failure.
 */
int main(int nc, char *np[]) {
    int token, index;

    if (nc < 2) {
        fprintf(stderr, "File name id not given.\n");
        return EXIT_FAILURE;
    }
    if (init_scan(np[1]) < 0) {
        fprintf(stderr, "File %s can not open.\n", np[1]);
        return EXIT_FAILURE;
    }

    memset(numtoken, 0, sizeof(numtoken));
    init_idtab();

    while ((token = scan()) >= 0) {
        /* Count the tokens */
        numtoken[token]++;
        /* Count by name */
        if (token == TNAME) {
            id_countup(string_attr);
        }
    }

    if (end_scan() < 0) {
        fprintf(stderr, "File %s can not close.\n", np[1]);
        return EXIT_FAILURE;
    }
    /* Output the results of the count. */
    for (index = 0; index < NUMOFTOKEN + 1; index++) {
        if (numtoken[index] > 0) {
            fprintf(stdout, "%10s: %5d\n", tokenstr[index], numtoken[index]);
        }
        if (index == TNAME) {
            print_idtab();
        }
    }
    release_idtab();

    return 0;
}

/*!
 * @brief display an error message
 * @param[in] mes Error message
 */
void error(char *mes) {
    fprintf(stderr, "\n ERROR: %s\n", mes);
}
