#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>

TEST test_close_stdout_basic(void) {
    exec_capture("rm -rf a b c d e", NULL, 0);
    exec_capture("touch a", NULL, 0);
    
    ASSERT_EQ(0, exec_capture("cp a b >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("test -f b", NULL, 0));
    ASSERT_EQ(0, exec_capture("chmod o-w . >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("ln a c >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("rm c >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("mkdir d >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("mv d e >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("rmdir e >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("touch e >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("sleep 0 >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("true >&-", NULL, 0));
    ASSERT_EQ(0, exec_capture("printf '' >&-", NULL, 0));

#ifndef _WIN32
    // If /dev/full is available
    struct stat st;
    if (stat("/dev/full", &st) == 0 && S_ISCHR(st.st_mode)) {
        int r = exec_capture("printf 'foo' >/dev/full 2>/dev/null", NULL, 0);
        ASSERT_EQ(1, WEXITSTATUS(r));
        
        r = exec_capture("cp --verbose a b >/dev/full 2>/dev/null", NULL, 0);
        ASSERT_EQ(1, WEXITSTATUS(r));
        
        r = exec_capture("mktemp -d tmpdir-XXXXXX >/dev/full 2>/dev/null", NULL, 0);
        ASSERT_EQ(1, WEXITSTATUS(r));
        
        r = exec_capture("mktemp -d -q tmpdir-XXXXXX >/dev/full 2>/dev/null", NULL, 0);
        ASSERT_EQ(1, WEXITSTATUS(r));
    }
#endif
    PASS();
}

SUITE(close_stdout_suite) {
    RUN_TEST(test_close_stdout_basic);
}

DEFINE_TEST_MAIN(close_stdout_suite)
