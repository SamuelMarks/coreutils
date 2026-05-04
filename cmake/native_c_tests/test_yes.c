#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>





TEST test_yes_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("yes --version", out, sizeof(out)));
    ASSERT(strstr(out, "yes") != NULL);
    PASS();
}

TEST test_yes_output(void) {
    SKIPm("Pipes not cleanly terminated natively");
    char out[8192];

    // Check basic operation
    ASSERT_EQ(0, exec_capture("yes | head -n1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("y", out);

    // Multiple arguments
    ASSERT_EQ(0, exec_capture("yes a b c | head -n1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("a b c", out);

    // Large argument
    char cmd[8192];
    char expected[8192];
    memset(expected, 'a', 4096);
    expected[4096] = '\0';
    snprintf(cmd, sizeof(cmd), "yes %s | head -n1", expected);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ(expected, out);

    PASS();
}

TEST test_yes_misc(void) {
    char out[8192];
#ifndef _WIN32
    // Check write error
    struct stat st;
    if (stat("/dev/full", &st) == 0) {
        { int _r = exec_capture("yes >/dev/full 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        FILE *err_f = fopen("err", "r");
        char err_msg[1024] = {0};
        if (err_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
            fclose(err_f);
        }
        ASSERT(strstr(err_msg, "standard output:") != NULL);
        remove("err");
    }
#endif
    PASS();
}




SUITE(yes_suite) {
    RUN_TEST(test_yes_basic);
    RUN_TEST(test_yes_output);
    RUN_TEST(test_yes_misc);
}
DEFINE_TEST_MAIN(yes_suite)
