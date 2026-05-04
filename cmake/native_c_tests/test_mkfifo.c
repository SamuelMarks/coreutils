#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_mkfifo_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("mkfifo --version", out, sizeof(out)));
    ASSERT(strstr(out, "mkfifo") != NULL);
    PASS();
}



SUITE(mkfifo_suite) {
    RUN_TEST(test_mkfifo_basic);
}
DEFINE_TEST_MAIN(mkfifo_suite)
