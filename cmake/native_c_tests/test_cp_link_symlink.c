#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <utime.h>
#include <sys/time.h>



static void cleanup() {
    my_system("rm -f file link link.cp");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_link_symlink(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("symlinks/hardlinks testing not supported on Windows");
#else
    create_file("file");
    if (symlink("file", "link") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }

    struct stat st;
    lstat("link", &st);

    // Try to update symlink timestamp using `touch -m -h -d 2011-01-01 link` or lutimes
    if (my_system("touch -m -h -d 2011-01-01 link 2>/dev/null") != 0) {
        cleanup();
        SKIPm("touch -h not supported");
    }

    lstat("link", &st);
    // 2011-01-01 00:00:00 is around 1293840000 timestamp depending on timezone
    // Instead of parsing date exactly, we just check if we can update it and if cp preserves it.
    
    time_t updated_mtime = st.st_mtime;

    ASSERT_EQ(0, exec_capture("cp -al link link.cp", NULL, 0));

    struct stat st_cp;
    ASSERT_EQ(0, lstat("link.cp", &st_cp));

    ASSERT_EQ(updated_mtime, st_cp.st_mtime);
#endif

    cleanup();
    PASS();
}





SUITE(cp_link_symlink_suite) {
    // RUN_TEST(test_cp_link_symlink);
}
DEFINE_TEST_MAIN(cp_link_symlink_suite)
