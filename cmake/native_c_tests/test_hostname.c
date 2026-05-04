#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_hostname_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("hostname", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}



SUITE(hostname_suite) {
    RUN_TEST(test_hostname_basic);
}
DEFINE_TEST_MAIN(hostname_suite)
