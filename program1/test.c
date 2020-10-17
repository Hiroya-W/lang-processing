#include <CUnit/CUnit.h>
/* #include <BCUnit/Console.h> */
#include <CUnit/Basic.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <stdio.h>

/* Source Files */
#include "scan.c"
#include "token-list.c"
#include "token-list.h"

void scan_func_test_isblank(void);
void integration_test_sample11pp(void);

void set_correct_ans_sample11pp(int *correct_ans);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Scan functions Test ", NULL, NULL);
    CU_add_test(suite, "scan_func_test_isblank", scan_func_test_isblank);

    suite = CU_add_suite("Integration Test ", NULL, NULL);
    CU_add_test(suite, "integration_test_sample11pp", integration_test_sample11pp);
    /* CU_console_run_tests(); */
    CU_basic_run_tests();

    int ret = CU_get_number_of_failures();
    printf("failed %d\n", ret);

    CU_cleanup_registry();

    if (ret != 0) {
        return ret;
    } else {
        return 0;
    }
}

void scan_func_test_isblank(void) {
    CU_ASSERT(_isblank(' ') == 1);
    CU_ASSERT(_isblank('\t') == 1);
    CU_ASSERT(_isblank('\n') == 0);
    CU_ASSERT(_isblank('a') == 0);
}

void integration_test_sample11pp(void) {
    int correct_ans[NUMOFTOKEN + 1];
    memset(correct_ans, 0, sizeof(correct_ans));
    set_correct_ans_sample11pp(correct_ans);

    int token, index;
    int ret;
    char *filename = "samples/sample11pp.mpl";
    init_scan(filename);

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
        numtoken[token]++;
    }

    ret = end_scan();
    CU_ASSERT_EQUAL(ret, 0); /* success -> 0, fail -> not 0  */

    if (ret < 0) {
        CU_FAIL("File sample11pp can not close.\n");
        return;
    }

    /* check */
    for (index = 0; index < NUMOFTOKEN + 1; index++) {
        fprintf(stdout, "%10s: %5d, correct == %5d\n", tokenstr[index], numtoken[index], correct_ans[index]);
        CU_ASSERT_EQUAL(numtoken[index], correct_ans[index]);
    }
}

void set_correct_ans_sample11pp(int *correct_ans) {
    correct_ans[TNAME] = 27;
    correct_ans[TPROGRAM] = 1;
    correct_ans[TVAR] = 4;
    correct_ans[TBEGIN] = 5;
    correct_ans[TEND] = 5;
    correct_ans[TPROCEDURE] = 3;
    correct_ans[TCALL] = 3;
    correct_ans[TWHILE] = 1;
    correct_ans[TDO] = 1;
    correct_ans[TINTEGER] = 6;
    correct_ans[TREADLN] = 2;
    correct_ans[TWRITELN] = 2;
    correct_ans[TNUMBER] = 4;
    correct_ans[TSTRING] = 2;
    correct_ans[TPLUS] = 1;
    correct_ans[TMINUS] = 1;
    correct_ans[TSTAR] = 1;
    correct_ans[TGR] = 1;
    correct_ans[TLPAREN] = 8;
    correct_ans[TRPAREN] = 8;
    correct_ans[TASSIGN] = 3;
    correct_ans[TDOT] = 1;
    correct_ans[TCOMMA] = 3;
    correct_ans[TCOLON] = 6;
    correct_ans[TSEMI] = 17;
}
