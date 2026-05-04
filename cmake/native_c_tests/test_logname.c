#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_logname_basic(void) {
    char out[512];
    int res = exec_capture("logname", out, sizeof(out));
    if (res == 0) {
        trim_newline(out);
        ASSERT(strlen(out) > 0);
    }
    PASS();
}



SUITE(logname_suite) {
    RUN_TEST(test_logname_basic);
}
DEFINE_TEST_MAIN(logname_suite)
