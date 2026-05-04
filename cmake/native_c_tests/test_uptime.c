#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_uptime_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("uptime", out, sizeof(out)));
    ASSERT(strlen(out) > 0);
    PASS();
}



SUITE(uptime_suite) {
    RUN_TEST(test_uptime_basic);
}
DEFINE_TEST_MAIN(uptime_suite)
