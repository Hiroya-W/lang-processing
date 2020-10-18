#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token-list.h"

/*! File pointer of the loaded file */
FILE *fp;
/*! Scanned unsigned integer */
int num_attr = 0;
/*! Scanned string  */
char string_attr[MAXSTRSIZE];

/*! @name looka ahead */
/* @{ */
/*! The letters you just loaded. */
static int current_char;
/*! look-ahead character */
static int next_char = '\0';
static void look_ahead();
/* @} */
/*! Line number of the character you just loaded */
static int linenum = 1;
/*! The line number of the last token scanned */
static int token_linenum = 0;

static int _isblank(int c);
int get_linenum(void);
void set_token_linenum(void);
static int scan_alnum();
static int scan_digit();
static int scan_string();
static int scan_comment();
static int scan_symbol();
static int get_keyword_token_code(char *token);
static int string_attr_push_back(const char c);

/*!
 * @brief Initialization to begin scanning
 * @param[in] filename File name to scan
 * @return int Returns 0 on success and -1 on failure.
 */
int init_scan(char *filename) {
    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }

    look_ahead();
    look_ahead();

    return 0;
}

/*!
 * @brief Scan the file and return the token code
 * @return int Returns token code on success and -1 on failure.
 */
int scan(void) {
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
    set_token_linenum();
    return token_code;
}

/*!
 * @brief Return the line number of the last token scanned
 * @return int Return line number
 */
int get_linenum(void) {
    return token_linenum;
}

/*!
 * @brief Set the line number of the last token scanned
 */
void set_token_linenum(void) {
    token_linenum = linenum;
}

/*!
 * @brief The process of finishing the scan
 * @return int Returns 0 on success and -1 on failure.
 */
int end_scan(void) {
    if (fclose(fp) == EOF) {
        return -1;
    }
    return 0;
}

/*!
 * @brief Determine if a character is a space character or not.
 * @param[in] c Character to be determined
 * @return int Returns 1 for a blank character, 0 otherwise.
 */
static int _isblank(int c) {
    if (c == ' ' || c == '\t') {
        return 1;
    } else {
        return 0;
    }
}

/*!
 * @brief Scan one string of letters and numbers
 * @return int Returns token code on success and -1 on failure.
 */
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

/*!
 * @brief Scan one number sequence.
 * @return int Returns token code of number on success and -1 on failure.
 */
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

/*!
 * @brief Scan one string.
 * @return int Returns token code of string on success and -1 on failure.
 */

static int scan_string() {
    memset(string_attr, '\0', sizeof(string_attr));
    look_ahead();

    while (1) {
        if (!isprint(current_char)) {
            error("function scan_string()");
            fprintf(stderr, "[%c]0x%x is not graphic character.\n", current_char, current_char);
            return -1;
        }

        if (current_char == '\'' && next_char != '\'') {
            break;
        }

        if (current_char == '\'' && next_char == '\'') {
            look_ahead();
        }

        if (string_attr_push_back(current_char) == -1) {
            error("function scan_string()");
            return -1;
        }
        look_ahead();
    }
    look_ahead(); /* read '\'' */

    return TSTRING;
}

/*!
 * @brief Scan the annotation
 * @return int Returns 0 on success and -1 on failure.
 */
static int scan_comment() {
    if (current_char == '/' && next_char == '*') {
        look_ahead();
        look_ahead();
        while (current_char != EOF) {
            if (current_char == '*' && next_char == '/') {
                look_ahead();
                look_ahead();
                return 0;
            }
            look_ahead();
        }
    } else if (current_char == '{') {
        look_ahead();
        while (current_char != EOF) {
            if (current_char == '}') {
                look_ahead();
                return 0;
            }
            look_ahead();
        }
    }
    /* EOF */
    return -1;
}

/*!
 * @brief Scan one symbol
 * @return int Returns token code of symbol on success and -1 on failure.
 */
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
            if (current_char == '>') {
                look_ahead();
                return TNOTEQ;
            } else if (current_char == '=') {
                look_ahead();
                return TLEEQ;
            } else {
                return TLE;
            }
        case '>':
            if (current_char == '=') {
                look_ahead();
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
            if (current_char == '=') {
                look_ahead();
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
/*!
 * @brief Get the token code for a token
 * @param[in] token token to be determined
 * @return int If it is a keyword, it returns its token code, otherwise it returns the Name token code
 */
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

/*!
 * @brief Adding characters to the end of a scanned string
 * @param[in] c Characters to add
 * @return int Returns 0 on success and -1 on failure.
 */
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

/*!
 * @brief Pre-reading file
 */
static void look_ahead() {
    current_char = next_char;
    next_char = fgetc(fp);
    return;
}
