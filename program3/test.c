#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/TestRun.h>
#include <stdio.h>

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
void id_register_as_type_std_test(void);
void id_register_as_type_array_test1(void);
void id_register_as_type_array_test2(void);
void id_register_parameter_list(void);
void register_linenum_test(void);

void integration_test_sample31p(void);
void integration_test_sample032p(void);
void integration_test_sample33p(void);
void integration_test_sample34(void);
void integration_test_sample35(void);

void test_init(void);
void test_end(void);
void parse(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("id-list test", NULL, NULL);
    CU_add_test(suite, "id_register_to_tab_test", id_register_without_type_test);
    CU_add_test(suite, "std_type_test", std_type_test);
    CU_add_test(suite, "id_register_as_type_test", id_register_as_type_test);
    CU_add_test(suite, "id_register_as_type_std_test", id_register_as_type_std_test);
    CU_add_test(suite, "id_register_as_type_array_test1", id_register_as_type_array_test1);
    CU_add_test(suite, "id_register_as_type_array_test2", id_register_as_type_array_test2);
    CU_add_test(suite, "id_register_parameter_list", id_register_parameter_list);
    CU_add_test(suite, "register_linenum_test", register_linenum_test);

    suite = CU_add_suite("Integration Test", NULL, NULL);
    CU_add_test(suite, "integration_test_sample31p", integration_test_sample31p);
    CU_add_test(suite, "integration_test_sample032p", integration_test_sample032p);
    CU_add_test(suite, "integration_test_sample33p", integration_test_sample33p);
    CU_add_test(suite, "integration_test_sample34", integration_test_sample34);
    CU_add_test(suite, "integration_test_sample35", integration_test_sample35);

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

    test_init();

    /* global */
    id_register_without_type("GLOBAL NAME1");
    id_register_without_type("GLOBAL NAME2");

    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "GLOBAL NAME1", NULL));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "GLOBAL NAME2", NULL));

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

    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "LOCAL NAME1", "procedure_name"));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&id_without_type_root, "LOCAL NAME2", "procedure_name"));

    root = id_without_type_root;
    CU_ASSERT_STRING_EQUAL(root->name, "LOCAL NAME2");
    CU_ASSERT_STRING_EQUAL(root->procname, "procedure_name");
    CU_ASSERT_PTR_NOT_NULL(root->nextp);

    root = root->nextp;
    CU_ASSERT_STRING_EQUAL(root->name, "LOCAL NAME1");
    CU_ASSERT_STRING_EQUAL(root->procname, "procedure_name");
    CU_ASSERT_PTR_NULL(root->nextp);

    test_end();
    return;
}

/*!
 * @brief 標準型(INT CHAR BOOLEAN)の情報を作成するテスト
 */
void std_type_test(void) {
    struct TYPE *type;

    test_init();

    type = std_type(TPINT);
    CU_ASSERT_EQUAL(type->ttype, TPINT);
    CU_ASSERT_EQUAL(type->arraysize, 0);
    CU_ASSERT_PTR_NULL(type->etp);
    CU_ASSERT_PTR_NULL(type->paratp);

    free(type);
    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 2つの変数を同時に登録する
 */
void id_register_as_type_test(void) {
    struct TYPE *type;
    struct ID *root;

    test_init();

    /* global */
    id_register_without_type("GLOBAL NAME1");
    id_register_without_type("GLOBAL NAME2");
    // INT型として記号表に登録
    type = std_type(TPINT);
    id_register_as_type(&type);

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME1", NULL));
    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "GLOBAL NAME2", NULL));
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

    print_tab(crtabroot);

    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 異なる標準型の変数を1つずつ追加する
 */
void id_register_as_type_std_test(void) {
    struct TYPE *type;
    struct ID *root;

    test_init();

    /* global */
    id_register_without_type("INT NAME1");
    // INT型として記号表に登録
    type = std_type(TPINT);
    id_register_as_type(&type);

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "INT NAME1", NULL));
    root = globalidroot;
    CU_ASSERT_STRING_EQUAL(root->name, "INT NAME1");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);

    id_register_without_type("CHAR NAME2");
    // CHAR型として記号表に登録
    type = std_type(TPCHAR);
    id_register_as_type(&type);
    root = globalidroot;

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "CHAR NAME2", NULL));
    CU_ASSERT_STRING_EQUAL(root->name, "CHAR NAME2");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);

    print_tab(crtabroot);

    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 異なる配列型の変数を1つずつ追加する
 */
void id_register_as_type_array_test1(void) {
    struct TYPE *type;
    struct ID *root;

    test_init();

    /* global */
    id_register_without_type("INT NAME1");
    // INT型として記号表に登録
    num_attr = 10;  // 配列の要素数
    type = array_type(TPARRAYINT);
    id_register_as_type(&type);

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "INT NAME1", NULL));
    root = globalidroot;
    CU_ASSERT_STRING_EQUAL(root->name, "INT NAME1");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);

    id_register_without_type("CHAR NAME2");
    // CHAR型として記号表に登録
    type = array_type(TPARRAYCHAR);
    id_register_as_type(&type);
    root = globalidroot;

    CU_ASSERT_PTR_NOT_NULL(search_tab(&globalidroot, "CHAR NAME2", NULL));
    CU_ASSERT_STRING_EQUAL(root->name, "CHAR NAME2");
    CU_ASSERT_PTR_NULL(root->procname);
    CU_ASSERT_EQUAL(root->ispara, 0);
    CU_ASSERT_EQUAL(root->deflinenum, 0);
    CU_ASSERT_PTR_NULL(root->irefp);

    print_tab(crtabroot);

    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 要素数が1未満の時の宣言はエラー
 */
void id_register_as_type_array_test2(void) {
    test_init();

    file_name = "./samples/id_register_as_type_array_test2.mpl";

    init_scan(file_name);
    indent_level = 0;
    linenum = 1;
    token_linenum = 0;

    token = scan();
    CU_ASSERT_EQUAL(parse_program(), ERROR);
    end_scan();
    fprintf(stdout, "\n");
    fflush(stdout);

    print_tab(crtabroot);

    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 手続き名の型リストを作る
 */
void id_register_parameter_list(void) {
    struct TYPE *type;

    test_init();

    // 手続き名を登録する
    definition_procedure_name = true;
    id_register_without_type("procedure name");
    type = std_type(TPPROC);
    id_register_as_type(&type);
    definition_procedure_name = false;

    // 仮引数リストをつくる
    in_subprogram_declaration = true;
    set_procedure_name("procedure name");
    is_formal_parameter = true;
    id_register_without_type("INT1");
    type = std_type(TPINT);
    id_register_as_type(&type);

    id_register_without_type("CHAR1");
    type = std_type(TPCHAR);
    id_register_as_type(&type);

    CU_ASSERT_PTR_NOT_NULL(globalidroot->itp->paratp);

    print_tab(crtabroot);

    test_end();
}

/*!
 * @brief 型情報を付加し、記号表に追加するテスト
 * 参照された行番号を追加する
 */
void register_linenum_test(void) {
    struct TYPE *type;

    test_init();

    // 手続き名を登録する
    definition_procedure_name = true;
    id_register_without_type("procedure name");
    type = std_type(TPPROC);
    id_register_as_type(&type);
    definition_procedure_name = false;

    // 仮引数リストをつくる
    in_subprogram_declaration = true;
    set_procedure_name("procedure name");
    is_formal_parameter = true;
    id_register_without_type("INT1");
    type = std_type(TPINT);
    id_register_as_type(&type);

    id_register_without_type("CHAR1");
    type = std_type(TPCHAR);
    id_register_as_type(&type);

    token_linenum = 1;
    register_linenum("INT1");
    token_linenum = 2;
    register_linenum("CHAR1");
    register_linenum("INT1");
    token_linenum = 3;
    register_linenum("CHAR1");
    register_linenum("INT1");

    in_subprogram_declaration = false;
    token_linenum = 4;
    register_linenum("procedure name");

    print_tab(crtabroot);

    test_end();
}

void integration_test_sample31p(void) {
    test_init();

    file_name = "./samples/sample31p.mpl";
    parse();

    print_tab(crtabroot);

    test_end();
}

void integration_test_sample032p(void) {
    test_init();

    file_name = "./samples/sample032p.mpl";
    parse();

    print_tab(crtabroot);

    test_end();
}

void integration_test_sample33p(void) {
    test_init();

    file_name = "./samples/sample33p.mpl";
    parse();

    print_tab(crtabroot);

    test_end();
}

void integration_test_sample34(void) {
    test_init();

    file_name = "./samples/sample34.mpl";
    parse();

    print_tab(crtabroot);

    test_end();
}

void integration_test_sample35(void) {
    test_init();

    file_name = "./samples/sample35.mpl";
    parse();

    print_tab(crtabroot);

    test_end();
}

void test_init(void) {
    init_crtab();
    in_subprogram_declaration = false;
    in_variable_declaration = false;
    is_array_type = false;
    is_formal_parameter = false;
}

void test_end(void) {
    release_crtab();
}

void parse(void) {
    init_scan(file_name);
    indent_level = 0;
    linenum = 1;
    token_linenum = 0;

    token = scan();
    parse_program();
    end_scan();
    fprintf(stdout, "\n");
    fflush(stdout);
}
