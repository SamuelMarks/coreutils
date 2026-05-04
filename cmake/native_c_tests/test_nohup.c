#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>




TEST test_nohup_basic(void) {
    char out[512];
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "nohup %s/echo hello", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    ASSERT(strstr(out, "hello") != NULL || strstr(out, "nohup: ignoring input") != NULL);
    remove("nohup.out");
    PASS();
}

TEST test_nohup_advanced(void) {
    char out[1024];

#ifndef _WIN32
    // Missing program
    { int _r = exec_capture("nohup >/dev/null 2>&1", out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }
    
    // Missing program POSIXLY_CORRECT
    { int _r = exec_capture("env POSIXLY_CORRECT=1 nohup >/dev/null 2>&1", out, sizeof(out)); ASSERT_EQ(127, WEXITSTATUS(_r)); }

    // Invalid command
    { int _r = exec_capture("nohup no-such-command 2> err", out, sizeof(out)); ASSERT_EQ(127, WEXITSTATUS(_r)); }
    remove("err");
    remove("nohup.out");
    
    // Command is not executable
    FILE *f = fopen("k", "w");
    if (f) if (f) fclose(f);
    chmod("k", 0);
    { int _r = exec_capture("nohup ./k 2> err", out, sizeof(out)); ASSERT_EQ(126, WEXITSTATUS(_r)); }
    remove("k");
    remove("err");
    remove("nohup.out");
#endif

    PASS();
}




SUITE(nohup_suite) {
    RUN_TEST(test_nohup_basic);
    RUN_TEST(test_nohup_advanced);
}
DEFINE_TEST_MAIN(nohup_suite)
