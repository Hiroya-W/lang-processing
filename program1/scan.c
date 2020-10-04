#include <stdio.h>
#include <stdlib.h>

#include "token-list.h"

FILE *fp;

int init_scan(char *filename) {
    if ((fp = fopen(filename, "r")) == NULL) {
        return EXIT_FAILURE;
    }
    return 0;
}

int scan(void) {
    return 0;
}
int get_linenum(void) {
    return 0;
}
void end_scan(void) {
    fclose(fp);
    return;
}
