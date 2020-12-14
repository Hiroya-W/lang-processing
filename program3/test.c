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
    struct ID *root;

    init_crtab();
    /* global */
    id_register("GLOBAL NAME1");
    id_register("GLOBAL NAME2");

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME1"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME2"));

    root = globalidroot;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME2");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_PTR_NOT_NULL(root->nextp);

    root = root->nextp;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME1");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_PTR_NULL(root->nextp);

    /* local */
    in_subprogram_declaration = true;
    set_procedure_name("procedure_name");
    id_register("LOCAL NAME1");
    id_register("LOCAL NAME2");

    CU_ASSERT_PTR_NOT_NULL(search_tab(&localidroot, "LOCAL NAME1"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&localidroot, "LOCAL NAME2"));

    root = localidroot;
    CU_ASSERT_STRING_EQUAL(root->name, "LOCAL NAME2");
    CU_ASSERT_STRING_EQUAL(root->procname, "procedure_name");
    CU_ASSERT_PTR_NOT_NULL(root->nextp);

    root = root->nextp;
    CU_ASSERT_STRING_EQUAL(root->name, "LOCAL NAME1");
    CU_ASSERT_STRING_EQUAL(root->procname, "procedure_name");
    CU_ASSERT_PTR_NULL(root->nextp);

    release_crtab();
    return;
}
