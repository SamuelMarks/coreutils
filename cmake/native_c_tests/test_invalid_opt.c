#include "test_helper.h"
#include "greatest.h"

// List of standard coreutils binaries to test
const char *built_programs[] = {
    "arch", "b2sum", "base32", "base64", "basename", "basenc", "cat", "chcon",
    "chgrp", "chmod", "chown", "chroot", "cksum", "comm", "cp", "csplit", "cut",
    "date", "dd", "df", "dir", "dircolors", "dirname", "du", "echo", "env",
    "expand", "expr", "factor", "false", "fmt", "fold", "ginstall", "groups",
    "head", "hostid", "hostname", "id", "join", "kill", "link", "ln", "logname",
    "ls", "md5sum", "mkdir", "mkfifo", "mknod", "mktemp", "mv", "nice", "nl",
    "nohup", "nproc", "numfmt", "od", "paste", "pathchk", "pinky", "pr", "printenv",
    "printf", "ptx", "pwd", "readlink", "realpath", "rm", "rmdir", "runcon",
    "seq", "sha1sum", "sha224sum", "sha256sum", "sha384sum", "sha512sum", "shred",
    "shuf", "sleep", "sort", "split", "stat", "stdbuf", "stty", "sum", "sync",
    "tac", "tail", "tee", "test", "timeout", "touch", "tr", "true", "truncate",
    "tsort", "tty", "uname", "unexpand", "uniq", "unlink", "uptime", "users",
    "vdir", "wc", "who", "whoami", "yes"
};

TEST test_invalid_opt_all() {
    int num_progs = sizeof(built_programs) / sizeof(built_programs[0]);
    for (int i = 0; i < num_progs; i++) {
        const char *prog = built_programs[i];
        
        // Some programs have special expected exit codes
        int expected_exit = 1;
        if (strcmp(prog, "ls") == 0 || strcmp(prog, "dir") == 0 || strcmp(prog, "vdir") == 0 || 
            strcmp(prog, "test") == 0 || strcmp(prog, "tty") == 0 || strcmp(prog, "sort") == 0 ||
            strcmp(prog, "printenv") == 0) {
            expected_exit = 2;
        } else if (strcmp(prog, "chroot") == 0 || strcmp(prog, "env") == 0 || 
                   strcmp(prog, "nice") == 0 || strcmp(prog, "nohup") == 0 || 
                   strcmp(prog, "runcon") == 0 || strcmp(prog, "stdbuf") == 0 || 
                   strcmp(prog, "timeout") == 0) {
            expected_exit = 125;
        } else if (strcmp(prog, "echo") == 0 || strcmp(prog, "expr") == 0 || 
                   strcmp(prog, "printf") == 0 || strcmp(prog, "true") == 0 || strcmp(prog, "false") == 0) {
            expected_exit = 0; // true, false, echo, expr, printf don't fail on -/
        }

        char prog_path[256];
        snprintf(prog_path, sizeof(prog_path), "%s/%s", getenv("BIN_DIR") ? getenv("BIN_DIR") : ".", prog);
        if (access(prog_path, X_OK) != 0) {
            fprintf(stderr, "SKIP: %s not found\n", prog);
            continue;
        }
        
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "%s -/ 2>/dev/null >/dev/null", prog);
        
        int res = exec_capture(cmd, NULL, 0);
        int actual_exit = WIFEXITED(res) ? WEXITSTATUS(res) : -1;
        
        // stty and false don't even process standard invalid options the same way or might succeed/fail differently depending on platform/args.
        // false will always fail with 1, true will always succeed with 0.
        if (strcmp(prog, "false") == 0) expected_exit = 1;
        
        if (actual_exit != expected_exit) {
            fprintf(stderr, "FAIL: %s returned %d, expected %d\n", prog, actual_exit, expected_exit);
            ASSERT_EQ(expected_exit, actual_exit);
        }
    }
    PASS();
}

SUITE(invalid_opt_suite) {
    RUN_TEST(test_invalid_opt_all);
}

DEFINE_TEST_MAIN(invalid_opt_suite)
