#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

const char *writers[] = {
    "cksum --debug /dev/null",
    "date --debug -d 'now'",
    "du --max-depth=0 --summarize /dev/null",
    "env --debug true",
    "ginstall --strip-program=foo foo bar",
    "numfmt --debug 1",
    "od -w1 /dev/null",
    "printf 'foo' 'bar'",
    "sort --debug /dev/null",
    "stat --printf '\\\\' .",
    "tail -n0 --retry foo",
    "tr . '\\\\' < /dev/null",
    "wc -l --debug /dev/null",
    NULL
};

TEST test_warning_errors_dev_full(void) {
#ifndef _WIN32
    struct stat st;
    if (stat("/dev/full", &st) == 0 && S_ISCHR(st.st_mode)) {
        exec_capture("echo foo > foo", NULL, 0);
        char cmd[256];
        char out[1024];

        for (int i = 0; writers[i] != NULL; i++) {
            char prog_path[256];
            char prog_name[256];
            sscanf(writers[i], "%s", prog_name); // extract first word
            snprintf(prog_path, sizeof(prog_path), "%s/%s", getenv("BIN_DIR") ? getenv("BIN_DIR") : ".", prog_name);
            if (access(prog_path, X_OK) != 0) {
                continue;
            }
            snprintf(cmd, sizeof(cmd), "%s 2>err_tmp", writers[i]);
            int r = exec_capture(cmd, NULL, 0);
            
            struct stat st_err;
            if (stat("err_tmp", &st_err) == 0 && st_err.st_size > 0) {
                int expected = 1;
                if (strncmp(writers[i], "sort ", 5) == 0) expected = 2;
                if (strncmp(writers[i], "env ", 4) == 0) expected = 0;
                
                snprintf(cmd, sizeof(cmd), "%s 2>/dev/full", writers[i]);
                r = exec_capture(cmd, out, sizeof(out));
                if (WIFEXITED(r) && WEXITSTATUS(r) != expected) {
                    fprintf(stderr, "FAIL: %s returned %d, expected %d on /dev/full\n", writers[i], WEXITSTATUS(r), expected);
                    ASSERT_EQ(expected, WEXITSTATUS(r));
                }
            }
        }
        remove("err_tmp");
        remove("foo");
    }
#endif
    PASS();
}

SUITE(warning_errors_suite) {
    RUN_TEST(test_warning_errors_dev_full);
}

DEFINE_TEST_MAIN(warning_errors_suite)
