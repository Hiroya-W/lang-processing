#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token-list.h"

FILE *fp;
char string_attr[MAXSTRSIZE];
static int cbuf; /* look-ahead character */

static int scan_alnum();
static int get_keyword_token_code(char *token);
static void look_ahead();
static int string_attr_push_back(const char c);

int init_scan(char *filename) {
    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }

    look_ahead();

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
        if (cbuf == EOF) {
            return -1;
        } else if (isblank(cbuf)) { /* Space or Tab */
            look_ahead();
            continue;
        } else if (isalpha(cbuf)) { /* Name or Keyword */
            token_code = scan_alnum();
            break;
        } else if (isdigit(cbuf)) { /* Digit */
            break;
        } else {
            break;
        }
    }
    return token_code;
}
int get_linenum(void) {
    return 0;
}
int end_scan(void) {
    if (fclose(fp) == EOF) {
        return -1;
    }
    return 0;
}

static int scan_alnum() {
    memset(string_attr, "\0", sizeof(string_attr));
    string_attr[0] = cbuf;

    look_ahead();
    while (isalnum(cbuf)) {
        if (string_attr_push_back(cbuf) == -1) {
            error("function scan_alnum()");
            return -1;
        }
        look_ahead();
    }
    return get_keyword_token_code(string_attr);
}

static int get_keyword_token_code(char *token) {
    int index;
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
    cbuf = fgetc(fp);
    return;
}
