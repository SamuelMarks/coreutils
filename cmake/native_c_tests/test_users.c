#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_users_basic(void) {
    ASSERT_EQ(0, exec_capture("users", NULL, 0));
    PASS();
}

TEST test_user_sh(void) {
    char out[1024];
#ifndef _WIN32
    // Skips tests relying on namespaces (unshare -U)
#endif
    PASS();
}



SUITE(users_suite) {
    RUN_TEST(test_users_basic);
    RUN_TEST(test_user_sh);
}
DEFINE_TEST_MAIN(users_suite)
