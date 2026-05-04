#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a b");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_no_deref_link1(void) {
    cleanup();

    MKDIR("a");
    MKDIR("b");

    FILE *f = fopen("a/foo", "w");
    if (f) {
        fprintf(f, "bar\n");
        if (f) fclose(f);
    }

#ifndef _WIN32
    if (symlink("../a/foo", "b/foo") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }

    // Returns 1 since 'a/foo' and 'b/foo' are the same file
    ASSERT_NEQ(0, exec_capture("cp -d a/foo b 2>/dev/null", NULL, 0));

    char out[1024] = {0};
    f = fopen("a/foo", "r");
    if (f) {
        fread(out, 1, sizeof(out)-1, f);
        if (f) fclose(f);
    }

    ASSERT_STR_EQ("bar\n", out);
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_no_deref_link1_suite) {
    RUN_TEST(test_cp_no_deref_link1);
}
DEFINE_TEST_MAIN(cp_no_deref_link1_suite)
