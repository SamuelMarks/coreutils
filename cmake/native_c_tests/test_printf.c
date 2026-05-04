#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_printf_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("env printf \"%s %d\" hello 42", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("hello 42", out);
    PASS();
}

TEST test_printf_sh(void) {
    char out[512];
    
    ASSERT_EQ(0, exec_capture("env printf '\\e'", out, sizeof(out)));
    ASSERT_STR_EQ("\033", out);

    ASSERT_EQ(0, exec_capture("env printf -- 'foo\\n'", out, sizeof(out)));
    ASSERT_STR_EQ("foo\n", out);

    ASSERT_EQ(0, exec_capture("env printf '%b' 'a\\nb'", out, sizeof(out)));
    ASSERT_STR_EQ("a\nb", out);

    int ret = exec_capture("env printf '\\x' 2>/dev/null", out, sizeof(out)); ASSERT(WEXITSTATUS(ret) != 0);
    
    PASS();
}

TEST test_printf_quote(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("env printf '%q' 'a b'", out, sizeof(out)));
    ASSERT_STR_EQ("'a b'", out);
    
    ASSERT_EQ(0, exec_capture("env printf '%q' \"a'b\"", out, sizeof(out)));
    ASSERT_STR_EQ("\"a'b\"", out);
    PASS();
}

TEST test_printf_printf_cov(void) {
    SKIPm("Skipped: cov test requires more shell scaffolding");
    PASS();
}
TEST test_printf_printf_hex(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("env printf '%x\n' 255", out, sizeof(out)));
    ASSERT_STR_EQ("ff\n", out);
#endif
    PASS();
}
TEST test_printf_printf_indexed(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("env printf '%2$s %1$s\n' world hello", out, sizeof(out)));
    ASSERT_STR_EQ("hello world\n", out);
#endif
    PASS();
}
TEST test_printf_printf_mb(void) {
    SKIPm("Skipped: multi-byte test requires specific locale setup");
    PASS();
}
TEST test_printf_printf_surprise(void) {
    char out[1024];
#ifndef _WIN32
    // test %c with numeric argument shouldn't be cast incorrectly
    ASSERT_EQ(0, exec_capture("env printf '%c\n' 65", out, sizeof(out)));
    ASSERT_STR_EQ("6\n", out); // printf %c takes the first char of the string "65"
#endif
    PASS();
}
TEST test_printf_printf(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("env printf 'hello %s\n' world", out, sizeof(out)));
    ASSERT_STR_EQ("hello world\n", out);
#endif
    PASS();
}



SUITE(printf_suite) {
    RUN_TEST(test_printf_basic);
    RUN_TEST(test_printf_sh);
    RUN_TEST(test_printf_quote);
    RUN_TEST(test_printf_printf_cov);
    RUN_TEST(test_printf_printf_hex);
    RUN_TEST(test_printf_printf_indexed);
    RUN_TEST(test_printf_printf_mb);
    RUN_TEST(test_printf_printf_surprise);
    RUN_TEST(test_printf_printf);
}
DEFINE_TEST_MAIN(printf_suite)
