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

void parse(void);
void check(char *output, char *answer);
void integration_test_sample29p(void);
void integration_test_sample021(void);
void integration_test_sample022(void);
void integration_test_sample023(void);
void integration_test_sample024(void);
void integration_test_sample025(void);
void integration_test_sample026(void);
void integration_test_sample02a(void);
void integration_test_parse_formal_parameters(void);
void integration_test_parse_output_format(void);

#undef main
int main() {
    CU_pSuite suite;

    CU_initialize_registry();

    suite = CU_add_suite("Integration Test", NULL, NULL);
    CU_add_test(suite, "integration_test_sample29p", integration_test_sample29p);
    CU_add_test(suite, "integration_test_sample021", integration_test_sample021);
    CU_add_test(suite, "integration_test_sample022", integration_test_sample022);
    CU_add_test(suite, "integration_test_sample023", integration_test_sample023);
    CU_add_test(suite, "integration_test_sample024", integration_test_sample024);
    CU_add_test(suite, "integration_test_sample025", integration_test_sample025);
    CU_add_test(suite, "integration_test_sample026", integration_test_sample026);
    CU_add_test(suite, "integration_test_sample02a", integration_test_sample02a);
    CU_add_test(suite, "integration_test_parse_formal_parameters", integration_test_parse_formal_parameters);
    CU_add_test(suite, "integration_test_parse_output_format", integration_test_parse_output_format);

    CU_basic_run_tests();

    int ret = CU_get_number_of_failures();

    CU_cleanup_registry();

    if (ret != 0) {
        return ret;
    } else {
        return 0;
    }
}

void integration_test_sample021(void) {
    char *output = "./outputs/sample021.mpl";
    char *answer = "./answers/sample021.mpl";
    file_name = "./samples/sample021.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample022(void) {
    char *output = "./outputs/sample022.mpl";
    char *answer = "./answers/sample022.mpl";
    file_name = "./samples/sample022.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample023(void) {
    char *output = "./outputs/sample023.mpl";
    char *answer = "./answers/sample023.mpl";
    file_name = "./samples/sample023.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample024(void) {
    char *output = "./outputs/sample024.mpl";
    char *answer = "./answers/sample024.mpl";
    file_name = "./samples/sample024.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample025(void) {
    char *output = "./outputs/sample025.mpl";
    char *answer = "./answers/sample025.mpl";
    file_name = "./samples/sample025.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample026(void) {
    char *output = "./outputs/sample026.mpl";
    char *answer = "./answers/sample026.mpl";
    file_name = "./samples/sample026.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample02a(void) {
    char *output = "./outputs/sample02a.mpl";
    char *answer = "./answers/sample02a.mpl";
    file_name = "./samples/sample02a.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_sample29p(void) {
    char *output = "./outputs/sample29p.mpl";
    char *answer = "./answers/sample29p.mpl";

    file_name = "./samples/sample29p.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_parse_formal_parameters(void) {
    char *output = "./outputs/parse_formal_parameters.mpl";
    char *answer = "./answers/parse_formal_parameters.mpl";

    file_name = "./samples/parse_formal_parameters.mpl";

    parse();
    check(output, answer);

    return;
}

void integration_test_parse_output_format(void) {
    char *output = "./outputs/parse_output_format.mpl";
    char *answer = "./answers/parse_output_format.mpl";

    file_name = "./samples/parse_output_format.mpl";

    parse();
    check(output, answer);

    return;
}

void parse(void) {
    init_scan(file_name);
    indent_level = 0;
    token = scan();
    parse_program();
    end_scan();
    fprintf(stdout, "\n");
    fflush(stdout);
}

void check(char *output, char *answer) {
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
}
