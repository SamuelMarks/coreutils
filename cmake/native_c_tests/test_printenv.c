#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>





#ifdef _WIN32
#define SETENV(k, v) _putenv_s(k, v)
#define UNSETENV(k) _putenv_s(k, "")
#else
#define SETENV(k, v) setenv(k, v, 1)
#define UNSETENV(k) unsetenv(k)
#endif

TEST test_printenv_basic(void) {
    char out[512];
    SETENV("TEST_VAR_123", "test_value_456");
    ASSERT_EQ(0, exec_capture("printenv TEST_VAR_123", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("test_value_456", out);
    UNSETENV("TEST_VAR_123");
    PASS();
}

TEST test_printenv_missing(void) {
    ASSERT(exec_capture("printenv NON_EXISTENT_VAR_ABCDEF", NULL, 0) != 0);
    PASS();
}

TEST test_printenv_advanced(void) {
    char out[1024];

#ifndef _WIN32
    // env -- env vs env -- printenv output comparison
    // Skipping this specific grep -Ev because grep isn't guaranteed and it's shell specific
    // but we can test printing multiple variables.

    SETENV("ENV_TEST1", "a");
    SETENV("ENV_TEST2", "b");

    ASSERT_EQ(0, exec_capture("printenv ENV_TEST2 ENV_TEST1 ENV_TEST2", out, sizeof(out)));
    ASSERT_STR_EQ("b\na\nb\n", out);

    ASSERT_EQ(0, exec_capture("printenv ENV_TEST1 ENV_TEST2", out, sizeof(out)));
    ASSERT_STR_EQ("a\nb\n", out);

    UNSETENV("ENV_TEST2");
    
    // Exit status reflects missing variable, but remaining arguments processed.
    { int _r = exec_capture("printenv ENV_TEST2 ENV_TEST1", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_STR_EQ("a\n", out);

    { int _r = exec_capture("printenv ENV_TEST1 ENV_TEST2", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_STR_EQ("a\n", out);

    UNSETENV("ENV_TEST1");

    // Non-standard environment variable name
    ASSERT_EQ(0, exec_capture("env -- -a=b printenv -- -a", out, sizeof(out)));
    ASSERT_STR_EQ("b\n", out);

    // Silently reject invalid env-var names.
    { int _r = exec_capture("env a=b=c printenv a=b", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_STR_EQ("", out);
#endif

    PASS();
}

TEST test_printenv_sh(void) {
    char out[1024];
#ifndef _WIN32
    // single value
    { int _r = exec_capture("printenv ENV_MISSING 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    setenv("ENV_TEST_1", "a", 1);
    setenv("ENV_TEST_2", "b", 1);
    
    ASSERT_EQ(0, exec_capture("printenv ENV_TEST_2 ENV_TEST_1 ENV_TEST_2", out, sizeof(out)));
    ASSERT_STR_EQ("b\na\nb\n", out);
    
    { int _r = exec_capture("printenv ENV_TEST_1 ENV_MISSING 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_STR_EQ("a\n", out); // should still print a
    
    unsetenv("ENV_TEST_1");
    unsetenv("ENV_TEST_2");
#endif
    PASS();
}



SUITE(printenv_suite) {
    RUN_TEST(test_printenv_basic);
    RUN_TEST(test_printenv_missing);
    RUN_TEST(test_printenv_advanced);
    RUN_TEST(test_printenv_sh);
}
DEFINE_TEST_MAIN(printenv_suite)
