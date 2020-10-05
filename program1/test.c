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

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Scan Test ", NULL, NULL);
    CU_add_test(suite, "test_001", scan_test_001);
    CU_console_run_tests();
    CU_cleanup_registry();
    return 0;
}

void scan_test_001(void) {
    CU_ASSERT(_isblank(' ') == 1);
    CU_ASSERT(_isblank('\t') == 1);
}

void scan_test_002(void) {
    CU_ASSERT(get_keyword_token_code("and") == TAND);
    CU_ASSERT(get_keyword_token_code("array") == TARRAY);
}
