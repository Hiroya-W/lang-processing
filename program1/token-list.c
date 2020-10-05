#include "token-list.h"

#include <stdio.h>
#include <stdlib.h>

/* keyword list */
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

/* Token counter */
int numtoken[NUMOFTOKEN + 1];

/* string of each token */
char *tokenstr[NUMOFTOKEN + 1] = {
    "", "NAME", "program", "var", "array", "of", "begin",
    "end", "if", "then", "else", "procedure", "return", "call",
    "while", "do", "not", "or", "div", "and", "char",
    "integer", "boolean", "readln", "writeln", "true", "false", "NUMBER",
    "STRING", "+", "-", "*", "=", "<>", "<",
    "<=", ">", ">=", "(", ")", "[", "]",
    ":=", ".", ",", ":", ";", "read", "write",
    "break"};

int main(int nc, char *np[]) {
    int token, i;

    if (nc < 2) {
        fprintf(stderr, "File name id not given.\n");
        return EXIT_FAILURE;
    }
    if (init_scan(np[1]) < 0) {
        fprintf(stderr, "File %s can not open.\n", np[1]);
        return EXIT_FAILURE;
    }

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
    }

    if (end_scan() < 0) {
        fprintf(stderr, "File %s can not close.\n", np[1]);
        return EXIT_FAILURE;
    }
    /* 作成する部分:カウントした結果を出力する */
    return 0;
}

void error(char *mes) {
    fprintf(stderr, "\n ERROR: %s\n", mes);
    /* end_scan(); */
}
