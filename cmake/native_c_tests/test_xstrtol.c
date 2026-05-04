#include "test_helper.h"
#include <unistd.h>
#include <sys/wait.h>

TEST test_xstrtol_diagnostics() {
    char out[1024];
    char cmd[1024];
    int r;
    
    // Very large integer string
    const char *too_big = "999999999999999999999999999999999999999999999999999999999999999999999999999999999";
    
    snprintf(cmd, sizeof(cmd), "pr --pages=%sh", too_big);
    r = exec_capture(cmd, out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    if (strstr(out, "invalid suffix in --pages argument") == NULL) {
        fprintf(stderr, "GOT: '%s'\n", out);
        ASSERT(0);
    }

    snprintf(cmd, sizeof(cmd), "pr --pages=%s", too_big);
    r = exec_capture(cmd, out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    ASSERT(strstr(out, "too large") != NULL);

    r = exec_capture("pr --pages=x", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    ASSERT(strstr(out, "invalid --pages argument") != NULL);

    r = exec_capture("pr --pages=9x", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    ASSERT(strstr(out, "invalid page range") != NULL);

    PASS();
}

SUITE(xstrtol_suite) {
    RUN_TEST(test_xstrtol_diagnostics);
}

DEFINE_TEST_MAIN(xstrtol_suite)
