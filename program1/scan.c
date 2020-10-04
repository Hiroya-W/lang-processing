#include <stdio.h>
#include <stdlib.h>

#include "token-list.h"

FILE *fp;
static int cbuf; /* look-ahead character */

static void look_ahead();

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
    return 0;
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

static void look_ahead() {
    cbuf = fgetc(fp);
    ungetc(cbuf, fp);
    return;
}
