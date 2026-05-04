#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f foo slink no-file junk bar");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_r_vs_symlink(void) {
    cleanup();

    create_file("foo");

#ifndef _WIN32
    if (symlink("foo", "slink") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }
    symlink("no-such-file", "no-file");

    // cp -r no-file junk
    ASSERT_EQ(0, exec_capture("cp -r no-file junk 2>/dev/null", NULL, 0));

    // cp -r slink bar
    ASSERT_EQ(0, exec_capture("cp -r slink bar 2>/dev/null", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, lstat("bar", &st));
    ASSERT(S_ISLNK(st.st_mode));
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_r_vs_symlink_suite) {
    RUN_TEST(test_cp_r_vs_symlink);
}
DEFINE_TEST_MAIN(cp_r_vs_symlink_suite)
