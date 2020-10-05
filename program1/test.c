#include <BCUnit/BCUnit.h>
#include <BCUnit/Console.h>
#include <BCUnit/TestDB.h>
#include <stdio.h>

/* Source Files */
#include "scan.c"
#include "token-list.c"
#include "token-list.h"

void scan_test_001(void);
void scan_test_002(void);
void scan_test_003(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Scan Test ", NULL, NULL);
    CU_add_test(suite, "test_001", scan_test_001);
    CU_add_test(suite, "test_002", scan_test_002);
    CU_add_test(suite, "test_003", scan_test_003);
    CU_console_run_tests();
    CU_cleanup_registry();
    return 0;
}

void scan_test_001(void) {
    CU_ASSERT(_isblank(' ') == 1);
    CU_ASSERT(_isblank('\t') == 1);
    CU_ASSERT(_isblank('\n') == 0);
    CU_ASSERT(_isblank('a') == 0);
}

void scan_test_002(void) {
    int i;
    for (i = 0; i < KEYWORDSIZE; i++) {
        CU_ASSERT(get_keyword_token_code(key[i].keyword) == key[i].keytoken);
    }
    CU_ASSERT(get_keyword_token_code("ABCDE") == TNAME);
    CU_ASSERT(get_keyword_token_code("AB12") == TNAME);
}

void scan_test_003(void) {
    init_scan("samples/sample11.mpl");
    CU_ASSERT(scan() == TPROGRAM);
    CU_ASSERT(scan() == TNAME);
    end_scan();
}

void scan_test_004(void) {
    init_scan("samples/sample11.mpl");
    cbuf = 0x80; /* Not Graphic character */
    CU_ASSERT(scan() == -1);
    end_scan();
}
