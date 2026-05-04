#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a e");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_parent_perm(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("permissions testing not supported on Windows");
#else
    umask(022); // set a known umask

    my_system("mkdir -p a/b/c a/b/d e");
    create_file("a/b/c/foo");
    create_file("a/b/d/foo");

    ASSERT_EQ(0, exec_capture("cp -p --parents a/b/c/foo e", NULL, 0));

    // Make permissions of e/a different
    chmod("e/a", 0700); // changing from 0755
    chmod("e/a/b", 0700);

    ASSERT_EQ(0, exec_capture("cp -p --parents a/b/d/foo e", NULL, 0));

    struct stat st_src, st_dst;
    
    const char *dirs[] = {"a", "a/b", "a/b/d"};
    for (int i = 0; i < 3; i++) {
        char dst_path[256];
        snprintf(dst_path, sizeof(dst_path), "e/%s", dirs[i]);

        ASSERT_EQ(0, stat(dirs[i], &st_src));
        ASSERT_EQ(0, stat(dst_path, &st_dst));

        // ignore set-GID bit as per the shell test logic
        mode_t src_mode = st_src.st_mode & 0777;
        mode_t dst_mode = st_dst.st_mode & 0777;

        ASSERT_EQ(src_mode, dst_mode);
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_parent_perm_suite) {
    // RUN_TEST(test_cp_parent_perm);
}
DEFINE_TEST_MAIN(cp_parent_perm_suite)
