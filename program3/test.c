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

void id_register_without_type_test(void);
void std_type_test(void);
void id_register_as_type_test(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("id-list test", NULL, NULL);
    CU_add_test(suite, "id_register_to_tab_test", id_register_without_type_test);
    CU_add_test(suite, "std_type_test", std_type_test);
    CU_add_test(suite, "id_register_as_type_test", id_register_as_type_test);

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

/*!
 * @brief 型情報なしで名前を確保できるかテスト
 */
void id_register_without_type_test(void) {
    struct ID *root;
    in_subprogram_declaration = false;

    init_crtab();
    /* global */
    id_register_without_type("GLOBAL NAME1");
    id_register_without_type("GLOBAL NAME2");

    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "GLOBAL NAME1"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "GLOBAL NAME2"));

    root = id_without_type_root;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME2");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_PTR_NOT_NULL(root->nextp);

    root = root->nextp;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME1");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_PTR_NULL(root->nextp);

    release_crtab();
    /* local */
    in_subprogram_declaration = true;
    set_procedure_name("procedure_name");
    id_register_without_type("LOCAL NAME1");
    id_register_without_type("LOCAL NAME2");

    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "LOCAL NAME1"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "LOCAL NAME2"));

    root = id_without_type_root;
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

/*!
 * @brief 標準型(INT CHAR BOOLEAN)の情報を作成するテスト
 */
void std_type_test(void) {
    struct TYPE *type;
    in_subprogram_declaration = false;

    type = std_type(TPINT);
    CU_ASSERT_EQUAL(type->ttype, TPINT);
    CU_ASSERT_EQUAL(type->arraysize, 0);
    CU_ASSERT_PTR_NULL(type->etp);
    CU_ASSERT_PTR_NULL(type->paratp);

    free(type);
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 */
void id_register_as_type_test(void) {
    struct TYPE *type;
    struct ID *root;
    in_subprogram_declaration = false;

    init_crtab();
    /* global */
    id_register_without_type("GLOBAL NAME1");
    id_register_without_type("GLOBAL NAME2");
    // INT型として記号表に登録
    type = std_type(TPINT);
    id_register_as_type(&type);

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME1"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME2"));
    root = globalidroot;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME1");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);
    CU_ASSERT_PTR_NOT_NULL(root->nextp);

    root = root->nextp;
    CU_ASSERT_STRING_EQUAL(root->name, "GLOBAL NAME2");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);
    CU_ASSERT_PTR_NULL(root->nextp);

    print_tab(globalidroot);
}

void id_register_to_tab_test1(void) {
    struct ID *root;

    init_crtab();
    /* global */
    id_register_without_type("GLOBAL NAME1");
    id_register_without_type("GLOBAL NAME2");

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
    id_register_without_type("LOCAL NAME1");
    id_register_without_type("LOCAL NAME2");

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
