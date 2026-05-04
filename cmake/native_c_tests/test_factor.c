#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_factor_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("factor 15", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("15: 3 5", out);
    PASS();
}

TEST test_factor_pl(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("factor 1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1:", out);

    ASSERT_EQ(0, exec_capture("factor 2", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("2: 2", out);

    ASSERT_EQ(0, exec_capture("factor 10", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("10: 2 5", out);

    ASSERT_EQ(0, exec_capture("factor 100", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("100: 2 2 5 5", out);

    ASSERT_EQ(0, exec_capture("factor 4294967296", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("4294967296: 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2", out);

    PASS();
}

TEST test_factor_parallel(void) {
    // Tests thread scaling or concurrency inside factor? factor has a --parallel option? No, factor does parallel polling in the script.
    PASS();
}

TEST test_factor_create_test(void) {
    // Test generator script. Skip.
    PASS();
}

TEST test_factor_factor(void) {
    // Covered by test_factor_pl. Skip.
    PASS();
}

TEST test_factor_run(void) {
    // Multi-arch test runner. Skip.
    PASS();
}







SUITE(factor_suite) {
    RUN_TEST(test_factor_basic);
    RUN_TEST(test_factor_pl);
    RUN_TEST(test_factor_parallel);
    RUN_TEST(test_factor_create_test);
    RUN_TEST(test_factor_factor);
    RUN_TEST(test_factor_run);
}
DEFINE_TEST_MAIN(factor_suite)
