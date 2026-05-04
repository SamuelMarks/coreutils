#include "test_helper.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



TEST test_true_basic(void) {
    ASSERT_EQ(0, exec_capture("true", NULL, 0));
    PASS();
}

TEST test_false_basic(void) {
    ASSERT(exec_capture("false", NULL, 0) != 0);
    PASS();
}

TEST test_false_status_help(void) {
    ASSERT(exec_capture("false --help", NULL, 0) != 0);
    PASS();
}

TEST test_false_status_version(void) {
    ASSERT(exec_capture("false --version", NULL, 0) != 0);
    PASS();
}

TEST test_true_status_full(void) {
#ifndef _WIN32
    int fd = open("/dev/full", O_WRONLY);
    if (fd >= 0) {
        close(fd);
        // true should exit nonzero when it can't write --help or --version to /dev/full
        { int _r = exec_capture("true --version > /dev/full", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("true --help > /dev/full", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    }
#endif
    PASS();
}





SUITE(true_false_suite) {
    RUN_TEST(test_true_basic);
    RUN_TEST(test_false_basic);
    RUN_TEST(test_false_status_help);
    RUN_TEST(test_false_status_version);
    RUN_TEST(test_true_status_full);
}
DEFINE_TEST_MAIN(true_false_suite)
