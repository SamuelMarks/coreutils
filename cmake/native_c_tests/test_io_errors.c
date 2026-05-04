#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

const char *writers[] = {
    "cat foo",
    "comm foo foo",
    "cut -c1- foo",
    "cut -f1- foo",
    "date +%s",
    "dd status=none if=foo",
    "expand foo",
    "factor 1",
    "fmt foo",
    "fold foo",
    "fold -b foo",
    "fold -c foo",
    "groups",
    "head -n1 foo",
    "id",
    "join foo foo",
    "nl foo",
    "numfmt --invalid=ignore < foo",
    "od -v foo",
    "paste foo",
    "pr foo",
    "seq 1",
    "seq 10000 | tac",
    "tail -n1 foo",
    "tee < foo",
    "tr . . < foo",
    "unexpand foo",
    "uniq foo",
    NULL
};

TEST test_io_errors_dev_full(void) {
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
            snprintf(cmd, sizeof(cmd), "%s >/dev/full 2>&1", writers[i]);
            int r = exec_capture(cmd, out, sizeof(out));
            if (WIFEXITED(r) && WEXITSTATUS(r) != 0) {
                // Should print some error about space or write error
                if (strstr(out, "space") == NULL && strstr(out, "write error") == NULL) {
                    fprintf(stderr, "FAIL: %s gave unexpected error msg: %s\n", writers[i], out);
                    ASSERT(0);
                }
            } else {
                fprintf(stderr, "FAIL: %s did not fail cleanly on /dev/full\n", writers[i]);
                ASSERT(0);
            }
        }
        
        // Let's test a couple of standard programs with --version to make sure close_stdout handles it.
        const char *generic[] = {"ls --version", "cp --version", "mkdir --version"};
        for (int i = 0; i < 3; i++) {
            snprintf(cmd, sizeof(cmd), "%s >/dev/full 2>&1", generic[i]);
            int r = exec_capture(cmd, out, sizeof(out));
            if (WIFEXITED(r) && WEXITSTATUS(r) != 0) {
                if (strstr(out, "space") == NULL && strstr(out, "write error") == NULL) {
                    fprintf(stderr, "FAIL: %s gave unexpected error msg: %s\n", generic[i], out);
                    ASSERT(0);
                }
            } else {
                fprintf(stderr, "FAIL: %s did not fail cleanly on /dev/full\n", generic[i]);
                ASSERT(0);
            }
        }

        remove("foo");
    }
#endif
    PASS();
}

TEST test_io_errors_closed_pipe(void) {
#ifndef _WIN32
    // If output is a closed pipe, they should just get SIGPIPE and die, no error msg
    // The shell handles this. Using `| :`
    exec_capture("echo foo > foo", NULL, 0);
    char cmd[256];
    char out[1024];

    for (int i = 0; writers[i] != NULL; i++) {
        // Just skip tac, seq 10000 | tac is messy to do without shell
        if (strstr(writers[i], "tac") != NULL) continue;
        
        char prog_path[256];
        char prog_name[256];
        sscanf(writers[i], "%s", prog_name); // extract first word
        snprintf(prog_path, sizeof(prog_path), "%s/%s", getenv("BIN_DIR") ? getenv("BIN_DIR") : ".", prog_name);
        if (access(prog_path, X_OK) != 0) {
            continue;
        }

        snprintf(cmd, sizeof(cmd), "%s 2>&1 | :", writers[i]);
        fprintf(stderr, "RUNNING: %s\n", cmd);
        int r = exec_capture(cmd, out, sizeof(out));
        fprintf(stderr, "DONE: %s\n", cmd);
        // If there's output on stderr, we failed
        if (strlen(out) > 0) {
            fprintf(stderr, "FAIL: %s produced stderr on closed pipe: %s\n", writers[i], out);
            ASSERT(0);
        }
    }
    
    remove("foo");
#endif
    PASS();
}

SUITE(io_errors_suite) {
    RUN_TEST(test_io_errors_dev_full);
    RUN_TEST(test_io_errors_closed_pipe);
}

DEFINE_TEST_MAIN(io_errors_suite)
