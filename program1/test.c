#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <stdio.h>

/* Source Files */
#include "scan.c"
#include "token-list.c"
#include "token-list.h"

void scan_func_test_isblank(void);

void integration_test_sample11pp(void);
void integration_test_sample12(void);
void integration_test_sample15(void);
void integration_test_sample011(void);

void exceptional_test1(void);

void set_correct_ans_sample11pp(int *correct_ans);
void set_correct_ans_sample12(int *correct_ans);
void set_correct_ans_sample15(int *correct_ans);
void set_correct_ans_sample011(int *correct_ans);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Scan functions Test", NULL, NULL);
    CU_add_test(suite, "scan_func_test_isblank", scan_func_test_isblank);

    suite = CU_add_suite("Integration Test", NULL, NULL);
    CU_add_test(suite, "integration_test_sample11pp", integration_test_sample11pp);
    CU_add_test(suite, "integration_test_sample12", integration_test_sample12);
    CU_add_test(suite, "integration_test_sample15", integration_test_sample15);
    CU_add_test(suite, "integration_test_sample011", integration_test_sample011);
    CU_basic_run_tests();

    suite = CU_add_suite("Exceptional Test", NULL, NULL);
    CU_add_test(suite, "exceptional_test1", exceptional_test1);

    int ret = CU_get_number_of_failures();

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
        CU_ASSERT_EQUAL(numtoken[index], correct_ans[index]);
    }
}

void integration_test_sample12(void) {
    int correct_ans[NUMOFTOKEN + 1];
    memset(correct_ans, 0, sizeof(correct_ans));
    set_correct_ans_sample12(correct_ans);

    int token, index;
    int ret;
    char *filename = "samples/sample12.mpl";
    init_scan(filename);

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
        numtoken[token]++;
    }

    ret = end_scan();
    CU_ASSERT_EQUAL(ret, 0); /* success -> 0, fail -> not 0  */

    if (ret < 0) {
        CU_FAIL("File sample12 can not close.\n");
        return;
    }

    /* check */
    for (index = 0; index < NUMOFTOKEN + 1; index++) {
        CU_ASSERT_EQUAL(numtoken[index], correct_ans[index]);
    }
}

void integration_test_sample15(void) {
    int correct_ans[NUMOFTOKEN + 1];
    memset(correct_ans, 0, sizeof(correct_ans));
    set_correct_ans_sample15(correct_ans);

    int token, index;
    int ret;
    char *filename = "samples/sample15.mpl";
    init_scan(filename);

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
        numtoken[token]++;
    }

    ret = end_scan();
    CU_ASSERT_EQUAL(ret, 0); /* success -> 0, fail -> not 0  */

    if (ret < 0) {
        CU_FAIL("File sample15 can not close.\n");
        return;
    }

    /* check */
    for (index = 0; index < NUMOFTOKEN + 1; index++) {
        CU_ASSERT_EQUAL(numtoken[index], correct_ans[index]);
    }
}

void integration_test_sample011(void) {
    int correct_ans[NUMOFTOKEN + 1];
    memset(correct_ans, 0, sizeof(correct_ans));
    set_correct_ans_sample011(correct_ans);

    int token, index;
    int ret;
    char *filename = "samples/sample011.mpl";
    init_scan(filename);

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
        numtoken[token]++;
    }

    ret = end_scan();
    CU_ASSERT_EQUAL(ret, 0); /* success -> 0, fail -> not 0  */

    if (ret < 0) {
        CU_FAIL("File sample011 can not close.\n");
        return;
    }

    /* check */
    for (index = 0; index < NUMOFTOKEN + 1; index++) {
        /* fprintf(stdout, "%10s: %5d, correct == %5d\n", tokenstr[index], numtoken[index], correct_ans[index]); */
        CU_ASSERT_EQUAL(numtoken[index], correct_ans[index]);
    }
}

void exceptional_test1(void) {
    int token;
    int ret;
    char *filename = "samples/comment1.mpl";
    init_scan(filename);

    memset(numtoken, 0, sizeof(numtoken));

    while ((token = scan()) >= 0) {
        /* 作成する部分：トークンをカウントする */
        numtoken[token]++;
    }

    ret = end_scan();
    CU_ASSERT_EQUAL(ret, 0); /* success -> 0, fail -> not 0  */

    if (ret < 0) {
        CU_FAIL("File comment1.mpl can not close.\n");
        return;
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

void set_correct_ans_sample12(int *correct_ans) {
    correct_ans[TNAME] = 1;
    correct_ans[TPROGRAM] = 1;
    correct_ans[TBEGIN] = 1;
    correct_ans[TEND] = 1;
    correct_ans[TDOT] = 1;
    correct_ans[TSEMI] = 1;
}

void set_correct_ans_sample15(int *correct_ans) {
    correct_ans[TNAME] = 18;
    correct_ans[TPROGRAM] = 1;
    correct_ans[TVAR] = 1;
    correct_ans[TBEGIN] = 2;
    correct_ans[TEND] = 2;
    correct_ans[TWHILE] = 1;
    correct_ans[TDO] = 1;
    correct_ans[TINTEGER] = 1;
    correct_ans[TWRITELN] = 3;
    correct_ans[TNUMBER] = 11;
    correct_ans[TSTRING] = 10;
    correct_ans[TPLUS] = 1;
    correct_ans[TSTAR] = 5;
    correct_ans[TLE] = 1;
    correct_ans[TLPAREN] = 3;
    correct_ans[TRPAREN] = 3;
    correct_ans[TASSIGN] = 4;
    correct_ans[TDOT] = 1;
    correct_ans[TCOMMA] = 13;
    correct_ans[TCOLON] = 6;
    correct_ans[TSEMI] = 9;
}

void set_correct_ans_sample011(int *correct_ans) {
    correct_ans[TNAME] = 8;
    correct_ans[TPROGRAM] = 1;
    correct_ans[TVAR] = 1;
    correct_ans[TARRAY] = 1;
    correct_ans[TOF] = 1;
    correct_ans[TBEGIN] = 1;
    correct_ans[TEND] = 1;
    correct_ans[TIF] = 1;
    correct_ans[TTHEN] = 1;
    correct_ans[TELSE] = 1;
    correct_ans[TPROCEDURE] = 1;
    correct_ans[TRETURN] = 1;
    correct_ans[TCALL] = 1;
    correct_ans[TWHILE] = 1;
    correct_ans[TDO] = 1;
    correct_ans[TNOT] = 1;
    correct_ans[TOR] = 1;
    correct_ans[TDIV] = 1;
    correct_ans[TAND] = 1;
    correct_ans[TCHAR] = 1;
    correct_ans[TINTEGER] = 1;
    correct_ans[TBOOLEAN] = 1;
    correct_ans[TREADLN] = 1;
    correct_ans[TWRITELN] = 1;
    correct_ans[TTRUE] = 1;
    correct_ans[TFALSE] = 1;
    correct_ans[TNUMBER] = 19;
    correct_ans[TSTRING] = 2;
    correct_ans[TPLUS] = 1;
    correct_ans[TMINUS] = 11;
    correct_ans[TSTAR] = 1;
    correct_ans[TEQUAL] = 2;
    correct_ans[TNOTEQ] = 1;
    correct_ans[TGR] = 1;
    correct_ans[TGREQ] = 1;
    correct_ans[TLE] = 1;
    correct_ans[TLEEQ] = 2;
    correct_ans[TLPAREN] = 1;
    correct_ans[TRPAREN] = 1;
    correct_ans[TLSQPAREN] = 1;
    correct_ans[TRSQPAREN] = 1;
    correct_ans[TASSIGN] = 2;
    correct_ans[TDOT] = 1;
    correct_ans[TCOMMA] = 1;
    correct_ans[TCOLON] = 1;
    correct_ans[TSEMI] = 2;
    correct_ans[TREAD] = 1;
    correct_ans[TWRITE] = 1;
    correct_ans[TBREAK] = 2;
}
