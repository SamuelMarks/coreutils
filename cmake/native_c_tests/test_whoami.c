#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_whoami_basic(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("whoami", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}





SUITE(whoami_suite) {
    RUN_TEST(test_whoami_basic);
}
DEFINE_TEST_MAIN(whoami_suite)
