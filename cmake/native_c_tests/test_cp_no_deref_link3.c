#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f a b");
}

TEST test_cp_no_deref_link3(void) {
    cleanup();

    FILE *f = fopen("a", "w");
    if (f) {
        fprintf(f, "bar\n");
        if (f) fclose(f);
    }

#ifndef _WIN32
    if (symlink("a", "b") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }

    // It should fail because 'a' and 'b' are the same file
    ASSERT_NEQ(0, exec_capture("cp -d a b 2>/dev/null", NULL, 0));

    char out[1024] = {0};
    f = fopen("a", "r");
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





SUITE(cp_no_deref_link3_suite) {
    RUN_TEST(test_cp_no_deref_link3);
}
DEFINE_TEST_MAIN(cp_no_deref_link3_suite)
