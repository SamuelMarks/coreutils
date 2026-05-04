#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f dangling-slink d2");
}

TEST test_cp_link_no_deref(void) {
    cleanup();

#ifndef _WIN32
    if (symlink("no-such-file", "dangling-slink") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }

    // Prior to coreutils-6.0, this would fail on non-Linux kernels,
    // with link being applied to the dangling symlink.
    // Notice that some systems don't support hard linking symlinks (e.g., older macOS).
    // `cp --link --no-dereference` uses `linkat()` with `AT_SYMLINK_NOFOLLOW`.
    // If it fails with ENOTSUP, coreutils falls back or fails.
    // The test in the shell script just runs it and expects it to work if `cp` has the workaround or OS supports it.
    // Let's just run it as the script does.
    ASSERT_EQ(0, exec_capture("cp --link --no-dereference dangling-slink d2", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, lstat("d2", &st));
    ASSERT(S_ISLNK(st.st_mode));
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_link_no_deref_suite) {
    RUN_TEST(test_cp_link_no_deref);
}
DEFINE_TEST_MAIN(cp_link_no_deref_suite)
