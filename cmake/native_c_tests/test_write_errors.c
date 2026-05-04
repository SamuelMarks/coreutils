#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

const char *writers[] = {
    "cat /dev/zero",
    "comm -z /dev/zero /dev/zero",
    "cut -z -c1- /dev/zero",
    "cut -z -f1- /dev/zero",
    "cut -f1 /dev/zero",
    "cut -b1- /dev/zero",
    "cut -c1- /dev/zero",
    // "date +%${OFF64_T_MAX}c", // skipping date huge output
    "dd if=/dev/zero",
    "expand /dev/zero",
    "fmt /dev/zero",
    "fold /dev/zero",
    "fold -b /dev/zero",
    "fold -c /dev/zero",
    "head -z -n-1 /dev/zero",
    "join -a 1 -z /dev/zero /dev/null",
    "od -v /dev/zero",
    "paste /dev/zero",
    "pr /dev/zero",
    "seq inf",
    "shuf -i 0-1 -r",
    "tail -n+1 -z /dev/zero",
    "tee < /dev/zero",
    "tr . . < /dev/zero",
    "unexpand /dev/zero",
    "uniq -z -D /dev/zero",
    "yes",
    NULL
};

TEST test_write_errors_dev_full(void) {
#ifndef _WIN32
    struct stat st;
    if (stat("/dev/full", &st) == 0 && S_ISCHR(st.st_mode)) {
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
            snprintf(cmd, sizeof(cmd), "timeout 2 %s >/dev/full 2>&1", writers[i]);
            int r = exec_capture(cmd, out, sizeof(out));
            // timeout returns 124 on timeout. If it times out, it failed to diagnose.
            // If it returns 124, we fail.
            if (WIFEXITED(r) && WEXITSTATUS(r) == 124) {
                fprintf(stderr, "FAIL: %s did not exit on /dev/full, timed out\n", writers[i]);
                ASSERT(0);
            }
            if (WIFEXITED(r) && WEXITSTATUS(r) == 0) {
                fprintf(stderr, "FAIL: %s exited with 0 on /dev/full\n", writers[i]);
                ASSERT(0);
            }
        }
    }
#endif
    PASS();
}

TEST test_write_errors_closed_pipe(void) {
#ifndef _WIN32
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
        snprintf(cmd, sizeof(cmd), "timeout 2 %s 2>&1 | :", writers[i]);
        int r = exec_capture(cmd, out, sizeof(out));
        if (strlen(out) > 0) {
            fprintf(stderr, "FAIL: %s produced stderr on closed pipe: %s\n", writers[i], out);
            ASSERT(0);
        }
        // WEXITSTATUS depends on the shell, `| :` usually exits 0
    }
#endif
    PASS();
}

SUITE(write_errors_suite) {
    RUN_TEST(test_write_errors_dev_full);
    RUN_TEST(test_write_errors_closed_pipe);
}

DEFINE_TEST_MAIN(write_errors_suite)
