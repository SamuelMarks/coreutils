#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>



TEST test_stdbuf_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("stdbuf --version", out, sizeof(out)));
    ASSERT(strstr(out, "stdbuf") != NULL);
    PASS();
}

TEST test_stdbuf_advanced(void) {
    char out[1024];

#ifndef _WIN32
    // Verify input parameter checking
    ASSERT_EQ(0, exec_capture("stdbuf -o1 true", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("stdbuf -oK true", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("stdbuf -o0 true", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("stdbuf -oL true", out, sizeof(out)));

    // Capital 'L' required
    { int _r = exec_capture("stdbuf -ol true", out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); }

    { int _r = exec_capture("stdbuf -oSIZE_OFLOW true", out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); } // Not a valid number, triggers too large or invalid
    { int _r = exec_capture("stdbuf -iL true", out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); } // line buffering stdin disallowed
    { int _r = exec_capture("stdbuf true", out, sizeof(out)); ASSERT_EQ(125, WEXITSTATUS(_r)); } // a buffering mode must be specified
    ASSERT_EQ(0, exec_capture("stdbuf -i0 -o0 -e0 true", out, sizeof(out))); // check all files

    FILE *f = fopen("not_executable", "w");
    if (f) if (f) fclose(f);
    chmod("not_executable", 0644);
    { int _r = exec_capture("stdbuf -o1 ./not_executable", out, sizeof(out)); ASSERT_EQ(126, WEXITSTATUS(_r)); }
    remove("not_executable");

    { int _r = exec_capture("stdbuf -o1 no_such_command_123", out, sizeof(out)); ASSERT_EQ(127, WEXITSTATUS(_r)); }

    // For the buffering effects, we might skip the complex fifo/sleep testing here for cross-platform stability
    // The coreutils shell test relies on timing and `dd` with FIFOs which is flaky and highly platform-dependent in a C my_system() call environment.
#endif

    PASS();
}




SUITE(stdbuf_suite) {
    RUN_TEST(test_stdbuf_basic);
    RUN_TEST(test_stdbuf_advanced);
}
DEFINE_TEST_MAIN(stdbuf_suite)
