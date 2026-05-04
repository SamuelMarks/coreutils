#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f file a b c d");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_slink_2_slink(void) {
    cleanup();

    create_file("file");

#ifndef _WIN32
    if (symlink("file", "a") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }
    symlink("file", "b");
    symlink("no-such-file", "c");
    symlink("no-such-file", "d");

    ASSERT_EQ(0, exec_capture("cp --update --no-dereference a b", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp --update --no-dereference c d", NULL, 0));
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_slink_2_slink_suite) {
    RUN_TEST(test_cp_slink_2_slink);
}
DEFINE_TEST_MAIN(cp_slink_2_slink_suite)
