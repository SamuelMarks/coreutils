#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_uname_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("uname", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}



SUITE(uname_suite) {
    RUN_TEST(test_uname_basic);
}
DEFINE_TEST_MAIN(uname_suite)
