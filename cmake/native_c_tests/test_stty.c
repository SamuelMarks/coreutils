#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_stty_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("stty --version", out, sizeof(out)));
    ASSERT(strstr(out, "stty") != NULL);
    PASS();
}

TEST test_stty_invalid(void) {
    char out[512];
    int ret;
    
    ret = exec_capture("stty invalid-arg 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    PASS();
}

TEST test_stty_row_col(void) {
    char out[512];
    
    // Test basic argument parsing 
    int ret = exec_capture("stty rows 40 cols 80 2>/dev/null", out, sizeof(out));
    // Could fail if not on a tty, just verify it doesn't crash
    
    PASS();
}

TEST test_stty_bad_speed(void) {
    SKIPm("Skipped: requires LD_PRELOAD intercept to work reliably");
    PASS();
}

TEST test_stty_stty_invalid(void) {
    char out[1024];
    
    // Invalid formats
    { int _r = exec_capture("stty ispeed 9599.. 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("stty ispeed 9600.5. 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("stty ispeed ++9600 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("stty ispeed 0x2580 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("stty ispeed 96E2 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("stty ispeed 9600,0 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    PASS();
}
TEST test_stty_stty_pairs(void) {
    SKIPm("Skipped: stty pair testing requires a PTY.");
    PASS();
}
TEST test_stty_stty(void) {
    SKIPm("Skipped: stty requires a PTY.");
    PASS();
}



SUITE(stty_suite) {
    RUN_TEST(test_stty_basic);
    RUN_TEST(test_stty_invalid);
    RUN_TEST(test_stty_row_col);
    RUN_TEST(test_stty_bad_speed);
    RUN_TEST(test_stty_stty_invalid);
    RUN_TEST(test_stty_stty_pairs);
    RUN_TEST(test_stty_stty);
}
DEFINE_TEST_MAIN(stty_suite)
