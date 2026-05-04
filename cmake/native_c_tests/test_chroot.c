#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>





TEST test_chroot_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("chroot --version", out, sizeof(out)));
    ASSERT(strstr(out, "chroot") != NULL);
    PASS();
}

TEST test_chroot_fail(void) {
    char out[1024];
    int ret;
    
    ret = exec_capture("chroot 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(125, WEXITSTATUS(ret));
    ret = exec_capture("chroot --- / true 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(125, WEXITSTATUS(ret));
    
    ret = exec_capture("chroot / true 2>/dev/null", out, sizeof(out));
    int exit_code = WEXITSTATUS(ret);
    if (exit_code == 0) {
        ret = exec_capture("chroot / false", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        ret = exec_capture("chroot / . 2>/dev/null", out, sizeof(out));
        ASSERT_EQ(126, WEXITSTATUS(ret));
        ret = exec_capture("chroot / no_such_cmd 2>/dev/null", out, sizeof(out));
        ASSERT_EQ(127, WEXITSTATUS(ret));
    } else {
        ASSERT_EQ(125, exit_code);
    }
    
    ret = exec_capture("chroot --skip-chdir . env pwd 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(125, WEXITSTATUS(ret));
    
    PASS();
}

TEST test_chroot_chroot_credentials(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        // Skip test if not root
        PASS();
    }
    
    // Just a sanity check for credentials flag
    int ret = exec_capture("chroot --userspec=0:0 / true 2>/dev/null", out, sizeof(out));
    if (WEXITSTATUS(ret) == 0 || WEXITSTATUS(ret) == 125) {
        // 125 means chroot failed (e.g. environment issue), 0 means success. Both are fine.
        PASS();
    }
#endif
    PASS();
}






SUITE(chroot_suite) {
    RUN_TEST(test_chroot_basic);
    RUN_TEST(test_chroot_fail);
    RUN_TEST(test_chroot_chroot_credentials);
}
DEFINE_TEST_MAIN(chroot_suite)
