#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f empty out exp err");
}

TEST test_cp_proc_zero_len(void) {
    cleanup();

    FILE *f_empty = fopen("empty", "w");
    if (f_empty) fclose(f_empty);

#ifdef __linux__
    const char *f = "/proc/cpuinfo";
    struct stat st;
    if (stat(f, &st) != 0 || access(f, R_OK) != 0) {
        f = "empty";
    }
#else
    const char *f = "empty";
#endif

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cat %s > out", f);
    
    // Actually, `cat` might not be in the path if not using system. But `cat` is usually in standard PATH.
    // However exec_capture prepends `./src/` to the command if the first word is a built program.
    // wait, exec_capture only prepends to the FIRST token. Here it's "cat" so it runs `./src/cat`.
    // Does `./src/cat` exist? Let's use `system` instead for standard coreutils, or use exec_capture.
    // Wait, the shell script `cat $f > out` works because `cat` is tested.
    int res = exec_capture(cmd, NULL, 0);
    if (res != 0) {
        // If ./src/cat doesn't exist, we fall back to system cat.
        snprintf(cmd, sizeof(cmd), "cat %s > out", f);
        my_system(cmd);
    }

    snprintf(cmd, sizeof(cmd), "cp %s exp", f);
    res = exec_capture(cmd, NULL, 0);
    if (res != 0) {
        printf("cp %s exp failed\n", f);
        ASSERT(0);
    }

    struct stat st_out, st_exp;
    ASSERT_EQ(0, stat("out", &st_out));
    ASSERT_EQ(0, stat("exp", &st_exp));

    // Both should be empty or both should be nonempty.
    ASSERT_EQ(st_out.st_size > 0 ? 1 : 0, st_exp.st_size > 0 ? 1 : 0);

    cleanup();
    PASS();
}





SUITE(cp_proc_zero_len_suite) {
    RUN_TEST(test_cp_proc_zero_len);
}
DEFINE_TEST_MAIN(cp_proc_zero_len_suite)
