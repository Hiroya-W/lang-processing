#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token-list.h"

FILE *fp;
char string_attr[MAXSTRSIZE];
static int current_char;
static int next_char = '\0'; /* look-ahead character */

static int _isblank(int c);
static int scan_alnum();
static int scan_digit();
static int scan_string();
static int scan_comment();
static int scan_symbol();
static int get_keyword_token_code(char *token);
static void look_ahead();
static int string_attr_push_back(const char c);
static int linenum = 1;
static int token_linenum = 0;
int num_attr = 0;

int init_scan(char *filename) {
    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }

    look_ahead(); /* current_char = `\0`, next_char = 'p' */
    look_ahead(); /* current_char = `p`, next_char = 'r' */

    return 0;
}

int scan(void) {
    /* 
     * 先頭の文字が分離子であれば，それを読み飛ばす（注釈の時は注釈全体を読み飛ばす） 
     * 分離子以外の文字であれば，次のように場合分けする
     *   英字なら，英数字が続く限り読み込む．それがキーワードのどれかならそのキーワードである
     *     どのキーワードとも異なれば，名前である．
     *   数字なら，数字が続く限り読み込む．それは符号なし整数である．
     */
    int token_code = -1;
    while (1) {
        if (current_char == EOF) { /* End Of File*/
            return -1;
        } else if (current_char == '\r' || current_char == '\n') { /* End of Line */
            if (current_char == '\r') {
                if (next_char == '\n') {
                    look_ahead();
                }
                look_ahead();
                linenum++;
            } else {
                if (next_char == '\r') {
                    look_ahead();
                }
                look_ahead();
                linenum++;
            }
        } else if (_isblank(current_char)) { /* Separator (Space or Tab) */
            look_ahead();
        } else if (!isprint(current_char)) { /* Not Graphic Character(0x20~0x7e) */
            return -1;
        } else if (isalpha(current_char)) { /* Name or Keyword */
            token_code = scan_alnum();
            break;
        } else if (isdigit(current_char)) { /* Digit */
            token_code = scan_digit();
            break;
        } else if (current_char == '\'') { /* String */
            token_code = scan_string();
            break;
        } else if ((current_char == '/' && next_char == '*') || current_char == '{') { /* Comment */
            if (scan_comment() == -1) {
                break;
            }
        } else { /* Symbol */
            token_code = scan_symbol();
            break;
        }
    }
    return token_code;
}

int get_linenum(void) {
    return token_linenum;
}

void set_token_linenum(void) {
    token_linenum = linenum;
}

int end_scan(void) {
    if (fclose(fp) == EOF) {
        return -1;
    }
    return 0;
}

static int _isblank(int c) {
    if (c == ' ' || c == '\t') {
        return 1;
    } else {
        return 0;
    }
}

static int scan_alnum() {
    memset(string_attr, '\0', sizeof(string_attr));
    string_attr[0] = current_char;

    look_ahead();
    while (isalnum(current_char)) {
        if (string_attr_push_back(current_char) == -1) {
            error("function scan_alnum()");
            return -1;
        }
        look_ahead();
    }
    return get_keyword_token_code(string_attr);
}

static int scan_digit() {
    int num = current_char - '0';

    memset(string_attr, '\0', sizeof(string_attr));
    string_attr[0] = current_char;

    look_ahead();
    while (isdigit(current_char)) {
        if (string_attr_push_back(current_char) == -1) {
            error("function scan_digit()");
            return -1;
        }
        num *= 10;
        num += current_char - '0';
        look_ahead();
    }
    if (num <= MAX_NUM_ATTR) {
        num_attr = num;
        return TNUMBER;
    }

    return -1;
}

static int scan_string() {
    memset(string_attr, '\0', sizeof(string_attr));
    look_ahead();

    while ((!(current_char == '\'') || next_char == '\'')) {
        if (!isprint(current_char)) {
            error("function scan_string()");
            fprintf(stderr, "%x is not graphic character.\n", current_char);
            return -1;
        }
        if (string_attr_push_back(current_char) == -1) {
            error("function scan_digit()");
            return -1;
        }
        look_ahead();
    }
    return TSTRING;
}

static int scan_comment() {
    if (current_char == '/' && next_char == '*') {
        look_ahead();
        look_ahead();
        while (!(current_char != EOF)) {
            if (current_char == '*' && next_char == '/') {
                look_ahead();
                look_ahead();
                return 0;
            }
        }
    } else if (current_char == '{') {
        look_ahead();
        while (!(current_char != EOF)) {
            if (current_char == '}') {
                look_ahead();
                return 0;
            }
        }
    }
    /* EOF */
    return -1;
}

static int scan_symbol() {
    char symbol = current_char;
    look_ahead();
    switch (symbol) {
        case '+':
            return TPLUS;
        case '-':
            return TMINUS;
        case '*':
            return TSTAR;
        case '=':
            return TEQUAL;
        case '<':
            if (next_char == '>') {
                return TNOTEQ;
            } else if (next_char == '=') {
                return TLEEQ;
            } else {
                return TLE;
            }
        case '>':
            if (next_char == '=') {
                return TGREQ;
            } else {
                return TGR;
            }
        case '(':
            return TLPAREN;
        case ')':
            return TRPAREN;
        case '[':
            return TLSQPAREN;
        case ']':
            return TRSQPAREN;
        case ':':
            if (next_char == '=') {
                return TASSIGN;
            } else {
                return TCOLON;
            }
        case '.':
            return TDOT;
        case ',':
            return TCOMMA;
        case ';':
            return TSEMI;
        default:
            return -1;
    }
}

static int get_keyword_token_code(char *token) {
    int index;
    /* TODO: binary search */
    for (index = 0; index < KEYWORDSIZE; index++) {
        if (strcmp(token, key[index].keyword) == 0) {
            /* This token is Keyword */
            return key[index].keytoken;
        }
    }
    /* This token is NAME*/
    return TNAME;
}

static int string_attr_push_back(const char c) {
    int len = strlen(string_attr);
    if (len < MAXSTRSIZE - 1) {
        string_attr[len] = c;
        return 0;
    } else {
        /* Buffer Overflow */
        error("string_attr: Buffer Overflow.");
        return -1;
    }
}

static void look_ahead() {
    current_char = next_char;
    next_char = fgetc(fp);
    return;
}
