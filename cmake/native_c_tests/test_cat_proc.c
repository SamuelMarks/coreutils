#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>



static void cleanup() {
    my_system("rm -f out exp");
}

TEST test_cat_proc(void) {
    cleanup();

#ifdef __linux__
    const char *f = "/proc/cpuinfo";
    struct stat st;
    if (stat(f, &st) != 0 || access(f, R_OK) != 0) {
        SKIPm("system lacks readable /proc/cpuinfo");
    }

    char cmd[256];
    
    // Yes, parts of /proc/cpuinfo might change between cat runs.
    // The sed filter should help to avoid any spurious numeric differences.
    snprintf(cmd, sizeof(cmd), "%s/cat -E %s | sed 's/[0-9][0-9]*/D/g' | tr -d '$' > out", bin_dir, f);
    ASSERT_EQ(0, my_system(cmd));

    snprintf(cmd, sizeof(cmd), "%s/cat %s | sed 's/[0-9][0-9]*/D/g' | tr -d '$' > exp", bin_dir, f);
    ASSERT_EQ(0, my_system(cmd));

    // Compare exp and out
    ASSERT_EQ(0, my_system("cmp exp out"));
#else
    SKIPm("/proc/cpuinfo test not applicable outside Linux");
#endif

    cleanup();
    PASS();
}





SUITE(cat_proc_suite) {
    RUN_TEST(test_cat_proc);
}
DEFINE_TEST_MAIN(cat_proc_suite)
