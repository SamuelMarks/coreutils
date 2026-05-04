#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf dir symlink s");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_symlink_slash(void) {
    cleanup();

    MKDIR("dir");

#ifndef _WIN32
    if (symlink("dir", "symlink") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }

    ASSERT_EQ(0, exec_capture("cp -dR symlink/ s", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, lstat("s", &st));
    
    // Prior to fileutils-4.0q, the trailing slash was removed unconditionally
    // causing cp to reproduce the symlink.
    // Now, cp ends up dereferencing the symlink and copying the directory.
    ASSERT(S_ISDIR(st.st_mode));
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_symlink_slash_suite) {
    RUN_TEST(test_cp_symlink_slash);
}
DEFINE_TEST_MAIN(cp_symlink_slash_suite)
