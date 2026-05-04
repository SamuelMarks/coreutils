#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>





TEST test_nice_basic(void) {
    char out[512];
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "nice -n 0 %s/echo hello", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    ASSERT(strstr(out, "hello") != NULL);
    PASS();
}

TEST test_nice_fail(void) {
    char out[512];
    char cmd[512];

    snprintf(cmd, sizeof(cmd), "nice -n 1");
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }

    snprintf(cmd, sizeof(cmd), "nice ---");
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }

    snprintf(cmd, sizeof(cmd), "nice -n 1a");
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }

    snprintf(cmd, sizeof(cmd), "nice -n 1+2-3 nice");
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }

    snprintf(cmd, sizeof(cmd), "nice %s/false", bin_dir);
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    snprintf(cmd, sizeof(cmd), "nice no_such");
    { int _r = exec_capture(cmd, out, sizeof(out)); ASSERT_EQ(127, WEXITSTATUS(_r)); }

    PASS();
}

TEST test_nice_args(void) {
    char out[512];
    char cmd[512];
    
    // Check baseline niceness first
    ASSERT_EQ(0, exec_capture("nice", out, sizeof(out)));
    int base_nice = atoi(out);
    
    // Only run this test if we are starting at nice level 0
    if (base_nice != 0) {
        PASS(); // Skip essentially
    }

    struct {
        const char *args;
        int expected;
    } tests[] = {
        {"", 10},
        {"-1", 1},
        {"-12", 12},
        {"-1 -2", 2},
        {"-n 1", 1},
        {"-n 1 -2", 2},
        {"-n 1 -+12", 12},
        {"-2 -n 1", 1},
        {"-2 -n 12", 12},
        {"-+1", 1},
        {"-+12", 12},
        {"-+1 -+12", 12},
        {"-n +1", 1},
        // We skip negative niceness tests for portability because they require privileges
        // and behavior depends on OS and user
    };

    for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        snprintf(cmd, sizeof(cmd), "nice %s nice 2> err", tests[i].args);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        char expected_str[32];
        snprintf(expected_str, sizeof(expected_str), "%d\n", tests[i].expected);
        
        // out should be expected_str
        ASSERT_EQ(0, strcmp(out, expected_str));
    }
    
    remove("err");
    PASS();
}

TEST test_nice_nice_fail(void) {
    char out[1024];
#ifndef _WIN32
    // If not root, nice -n -1 should fail
    if (geteuid() != 0) {
        int ret = exec_capture("nice -n -1 true 2>/dev/null", out, sizeof(out));
        ASSERT_EQ(0, WEXITSTATUS(ret)); // it prints a warning but continues nice itself failed to set priority
    }
#endif
    PASS();
}
TEST test_nice_nice(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("nice -n 1 true", out, sizeof(out)));
    
    // Check if it passes arguments
    ASSERT_EQ(0, exec_capture("nice -n 1 echo 'hello'", out, sizeof(out)));
    ASSERT_STR_EQ("hello\n", out);
    
    // Command not found
    { int _r = exec_capture("nice no_such_command 2>/dev/null", out, sizeof(out)); ASSERT_EQ(127, WEXITSTATUS(_r)); }
    
    // Command exits with specific code
    { int _r = exec_capture("nice false", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
#endif
    PASS();
}



SUITE(nice_suite) {
    RUN_TEST(test_nice_basic);
    RUN_TEST(test_nice_fail);
    RUN_TEST(test_nice_args);
    RUN_TEST(test_nice_nice_fail);
    RUN_TEST(test_nice_nice);
}
DEFINE_TEST_MAIN(nice_suite)
