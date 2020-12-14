#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>

// clang-format off
#include "mppl_compiler.h"
#include "cross_reference.c"
#include "id-list.c"
#include "main.c"
#include "scan.c"
// clang-format on

void id_register_to_tab_test(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("id-list test", NULL, NULL);
    CU_add_test(suite, "id_register_to_tab_test", id_register_to_tab_test);

    CU_basic_run_tests();
    /* CU_console_run_tests(); */

    int ret = CU_get_number_of_failures();

    CU_cleanup_registry();

    if (ret != 0) {
        return ret;
    } else {
        return 0;
    }
}

void id_register_to_tab_test(void) {
    init_crtab();
    id_register("GLOBAL NAME");
    CU_ASSERT_STRING_EQUAL(globalidroot->name, "GLOBAL NAME");
    CU_ASSERT_STRING_EQUAL(globalidroot->procname, "");

    in_subprogram_declaration = true;
    set_procedure_name("procedure_name");
    id_register("LOCAL NAME");
    CU_ASSERT_STRING_EQUAL(localidroot->name, "LOCAL NAME");
    CU_ASSERT_STRING_EQUAL(localidroot->procname, "procedure_name");

    release_crtab();
    return;
}
