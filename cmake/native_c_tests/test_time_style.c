#include "test_helper.h"

TEST test_time_style_basic(void) {
    char out[1024];
    
    // time-style.sh logic (partial)
    exec_capture("echo hello > a", NULL, 0);
    exec_capture("env TZ=UTC0 touch -d '1970-07-08 09:10:11' a", NULL, 0);
    
    int _r = exec_capture("env TZ=UTC0 LC_ALL=C ls -no --time-style=full-iso a", out, sizeof(out));
    if (WEXITSTATUS(_r) != 0) {
        printf("FAILED with exit code %d\nOUTPUT: %s\n", WEXITSTATUS(_r), out);
    }
    ASSERT_EQ(0, WEXITSTATUS(_r));
    ASSERT(strstr(out, "1970-07-08 09:10:11.000000000 +0000") != NULL);
    
    _r = exec_capture("env TZ=UTC0 LC_ALL=C ls -no --time-style=long-iso a", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(_r));
    ASSERT(strstr(out, "1970-07-08 09:10") != NULL);
    
    _r = exec_capture("env TZ=UTC0 LC_ALL=C ls -no --time-style=iso a", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(_r));
    ASSERT(strstr(out, "1970-07-08") != NULL);
    
    // test du time-style
    _r = exec_capture("env TZ=UTC0 LC_ALL=C du --time --time-style=iso a", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(_r));
    ASSERT(strstr(out, "1970-07-08") != NULL);
    
    PASS();
}

SUITE(time_style_suite) {
    RUN_TEST(test_time_style_basic);
}
DEFINE_TEST_MAIN(time_style_suite)
