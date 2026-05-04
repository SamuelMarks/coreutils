#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

const char *readers[] = {
    "basenc --base32 .",
    "basenc -d --base64 .",
    "cat . .",
    "cksum -a blake2b . .",
    "cksum -a crc . .",
    "cksum -a md5 . .",
    "cksum -a sha1 . .",
    "cksum -a sha2 -l 256 . .",
    "comm . .",
    "csplit . 1",
    "cut -b1 . .",
    "cut -f1 . .",
    "date -f .",
    "dd if=.",
    "dircolors .",
    "expand . .",
    "factor < .",
    "fmt . .",
    "fold . .",
    "head -n1 . .",
    "head -n-1 . .",
    "head -c1 . .",
    "head -c-1 . .",
    "join . .",
    "nl . .",
    "numfmt < .",
    "od . .",
    "paste . .",
    "pr .",
    "ptx .",
    "shuf -r .",
    "shuf -n1 .",
    "sort .",
    "split -l1 .",
    "split -b1 .",
    "split -C1 .",
    "split -n1 .",
    "split -nl/1 .",
    "split -nr/1 .",
    "tac . .",
    "tail -n1 . .",
    "tail -c1 . .",
    "tail -n+1 . .",
    "tail -c+1 . .",
    "tee < .",
    "tr 1 1 < .",
    "tsort .",
    "unexpand . .",
    "uniq .",
    "uniq -c .",
    "wc . .",
    "wc -c . .",
    "wc -l . .",
    NULL
};

TEST test_read_errors_basic(void) {
    char cmd[256];
    char out[1024];

    // Check if reading a directory fails on this system.
    int r = exec_capture("cat . 2>/dev/null", NULL, 0);
    if (r == 0) {
        SKIPm("Reading directory does not fail on this OS");
    }

    for (int i = 0; readers[i] != NULL; i++) {
        // Skip some utilities if they are not compiled, but we don't have a direct way
        // other than trying it. We just run them all, and if they return 0, we fail.
        
        char prog_path[256];
        char prog_name[256];
        sscanf(readers[i], "%s", prog_name); // extract first word
        snprintf(prog_path, sizeof(prog_path), "%s/%s", getenv("BIN_DIR") ? getenv("BIN_DIR") : ".", prog_name);
        if (access(prog_path, X_OK) != 0) {
            fprintf(stderr, "SKIP: %s not found\n", prog_name);
            continue;
        }

        snprintf(cmd, sizeof(cmd), "%s 2>&1", readers[i]);
        int ret = exec_capture(cmd, out, sizeof(out));
        if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
            // Some might not be compiled (e.g. they return 127 from our fake shell, but execvp might return 255 etc)
            // Wait, my_system returns what the shell returns.
            // If it's a "command not found" (127), we ignore it.
            fprintf(stderr, "FAIL: %s exited with 0 (output: %s)\n", readers[i], out);
            ASSERT(0);
        }
    }
    PASS();
}

SUITE(read_errors_suite) {
    RUN_TEST(test_read_errors_basic);
}

DEFINE_TEST_MAIN(read_errors_suite)
