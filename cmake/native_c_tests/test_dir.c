#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_dir_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("dir --version", out, sizeof(out)));
    ASSERT(strstr(out, "dir") != NULL);
    PASS();
}



SUITE(dir_suite) {
    RUN_TEST(test_dir_basic);
}
DEFINE_TEST_MAIN(dir_suite)
