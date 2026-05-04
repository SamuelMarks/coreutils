#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sleep_basic(void) {
    ASSERT_EQ(0, exec_capture("sleep 0.001", NULL, 0));
    PASS();
}

TEST test_sleep_invalid(void) {
    ASSERT(exec_capture("sleep invalid", NULL, 0) != 0);
    ASSERT(exec_capture("sleep -- -1", NULL, 0) != 0);
    ASSERT(exec_capture("sleep 42D", NULL, 0) != 0);
    ASSERT(exec_capture("sleep 42d 42day", NULL, 0) != 0);
    ASSERT(exec_capture("sleep nan", NULL, 0) != 0);
    ASSERT(exec_capture("sleep ''", NULL, 0) != 0);
    ASSERT(exec_capture("sleep", NULL, 0) != 0);
    PASS();
}

TEST test_sleep_advanced(void) {
    ASSERT_EQ(0, exec_capture("sleep 0x.002p1", NULL, 0));
    ASSERT_EQ(0, exec_capture("sleep 0x0.01d", NULL, 0));

    PASS();
}

TEST test_sleep_sh(void) {
    char out[1024];
    
    // Valid subsecond
    ASSERT_EQ(0, exec_capture("sleep 0.001", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("sleep 0x.002p1", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("sleep 0x0.01d", out, sizeof(out)));
    
    // Infinite / large
    { int _r = exec_capture("timeout 0.1 sleep 1d 2h 3m 4s", out, sizeof(out)); ASSERT_EQ(124, WEXITSTATUS(_r)); }
    { int _r = exec_capture("timeout 0.1 sleep inf", out, sizeof(out)); ASSERT_EQ(124, WEXITSTATUS(_r)); }
    
    PASS();
}




SUITE(sleep_suite) {
    RUN_TEST(test_sleep_basic);
    RUN_TEST(test_sleep_invalid);
    RUN_TEST(test_sleep_advanced);
    RUN_TEST(test_sleep_sh);
}
DEFINE_TEST_MAIN(sleep_suite)
