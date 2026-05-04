#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_hostid_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("hostid", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}



SUITE(hostid_suite) {
    RUN_TEST(test_hostid_basic);
}
DEFINE_TEST_MAIN(hostid_suite)
