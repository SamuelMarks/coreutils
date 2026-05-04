#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>

TEST test_sync_basic(void) {
    char out[1024];
    ASSERT_EQ(0, exec_capture("sync", out, sizeof(out)));
    PASS();
}
TEST test_sync_sh(void) {
    PASS();
}
SUITE(sync_suite) {
    RUN_TEST(test_sync_basic);
    RUN_TEST(test_sync_sh);
}
DEFINE_TEST_MAIN(sync_suite)
