#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <bits/types/FILE.h>
#include <stdio.h>
#include <string.h>

#include "mppl_compiler.h"
/* Source Files */
#include "main.c"
#include "pretty-printer.c"
#include "scan.c"

void integration_test_sample29p(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Integration Test", NULL, NULL);
    CU_add_test(suite, "integration_test_sample29p", integration_test_sample29p);

    CU_basic_run_tests();

    int ret = CU_get_number_of_failures();

    CU_cleanup_registry();

    if (ret != 0) {
        return ret;
    } else {
        return 0;
    }
}

void integration_test_sample29p(void) {
    char *output = "./outputs/sample29p.mpl";
    char *answer = "./answers/sample29p.mpl";

    FILE *fp_output;
    FILE *fp_answer;

    if ((fp_output = fopen(output, "r")) == NULL) {
        error("output fopen() returns NULL");
        return;
    }

    if ((fp_answer = fopen(answer, "r")) == NULL) {
        error("answer fopen() returns NULL");
        return;
    }

    char out_buf[1024];
    char ans_buf[1024];
    memset(out_buf, '\0', sizeof(out_buf));
    memset(ans_buf, '\0', sizeof(ans_buf));

    while (fgets(ans_buf, 1024, fp_answer) != NULL) {
        char *ret = fgets(out_buf, 1024, fp_output);
        if (ret == NULL) {
            CU_ASSERT_NOT_EQUAL(ret, NULL);
            return;
        }
        if (strcmp(out_buf, ans_buf) != 0) {
            CU_ASSERT(strcmp(out_buf, ans_buf) == 0);
            fprintf(stderr, "%s\n", out_buf);
            fprintf(stderr, "%s\n", ans_buf);
        }

        memset(out_buf, '\0', sizeof(out_buf));
        memset(ans_buf, '\0', sizeof(ans_buf));
    }

    return;
}
