#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a b c d");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_link_preserve(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("hardlinks testing not supported on Windows");
#else
    create_file("a");
    if (link("a", "b") != 0) {
        cleanup();
        SKIPm("hard links not supported");
    }
    MKDIR("c");

    ASSERT_EQ(0, exec_capture("cp -d a b c", NULL, 0));

    struct stat st_a, st_b;
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_EQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    cleanup();
    create_file("a");
    symlink("a", "b");
    MKDIR("c");
    ASSERT_EQ(0, exec_capture("cp --preserve=links -R -H a b c", NULL, 0));
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_EQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    cleanup();
    MKDIR("d");
    create_file("d/a");
    symlink("a", "d/b");
    ASSERT_EQ(0, exec_capture("cp --preserve=links -R -L d c", NULL, 0));
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_EQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    cleanup();
    MKDIR("d");
    create_file("d/a");
    link("d/a", "d/b");
    ASSERT_EQ(0, exec_capture("cp --preserve=links -R -L d c", NULL, 0));
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_EQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    // Ensure that --no-preserve=links works.
    cleanup();
    MKDIR("d");
    create_file("d/a");
    link("d/a", "d/b");
    ASSERT_EQ(0, exec_capture("cp -dR --no-preserve=links d c", NULL, 0));
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_NEQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    // Ensure that -d still preserves hard links.
    cleanup();
    create_file("a");
    link("a", "b");
    MKDIR("c");
    ASSERT_EQ(0, exec_capture("cp -d a b c", NULL, 0));
    ASSERT_EQ(0, stat("c/a", &st_a));
    ASSERT_EQ(0, stat("c/b", &st_b));
    ASSERT_EQ(st_a.st_ino, st_b.st_ino);

    // --------------------------------------
    // Ensure that --no-preserve=mode works
    cleanup();
    create_file("a");
    chmod("a", 0731);
    
    umask(077);
    ASSERT_EQ(0, exec_capture("cp -a --no-preserve=mode a b", NULL, 0));
    
    ASSERT_EQ(0, stat("b", &st_b));
    ASSERT_EQ((mode_t)0600, st_b.st_mode & 07777); // -rw-------
    
    umask(022);
#endif

    cleanup();
    PASS();
}





SUITE(cp_link_preserve_suite) {
    // RUN_TEST(test_cp_link_preserve);
}
DEFINE_TEST_MAIN(cp_link_preserve_suite)
