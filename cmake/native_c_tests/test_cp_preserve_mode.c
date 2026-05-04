#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a b c d1 d2 d3 fifo fifo_copy");
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

static mode_t get_mode(const char *f) {
    struct stat st;
    if (stat(f, &st) != 0) return 0;
    return st.st_mode & 07777;
}

TEST test_cp_preserve_mode(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("permissions testing not supported on Windows");
#else
    umask(0022);

    // regular file test
    create_file("a");
    create_file("b");
    chmod("b", 0600);
    ASSERT_EQ(0, exec_capture("cp --no-preserve=mode b c", NULL, 0));
    ASSERT_EQ(get_mode("a"), get_mode("c"));

    // existing destination test
    chmod("c", 0600);
    ASSERT_EQ(0, exec_capture("cp --no-preserve=mode a b", NULL, 0));
    ASSERT_EQ(get_mode("b"), get_mode("c"));

    // directory test
    my_system("mkdir d1 d2");
    chmod("d2", 0705);
    ASSERT_EQ(0, exec_capture("cp --no-preserve=mode -r d2 d3", NULL, 0));
    ASSERT_EQ(get_mode("d1"), get_mode("d3"));

    // contradicting options test
    unlink("a"); unlink("b");
    create_file("a");
    chmod("a", 0600);
    ASSERT_EQ(0, exec_capture("cp --no-preserve=mode --preserve=all a b", NULL, 0));
    ASSERT_EQ(get_mode("a"), get_mode("b"));

    // fifo test
    if (my_system("mkfifo fifo >/dev/null 2>&1") == 0) {
        ASSERT_EQ(0, exec_capture("cp -a --no-preserve=mode fifo fifo_copy", NULL, 0));
        ASSERT_EQ(get_mode("fifo"), get_mode("fifo_copy"));
    }

    // Test that plain --preserve=ownership does not affect destination mode.
    unlink("a"); unlink("b"); unlink("c");
    create_file("a");
    chmod("a", 0660);
    ASSERT_EQ(0, exec_capture("cp a b", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp --preserve=ownership a c", NULL, 0));
    ASSERT_EQ(get_mode("b"), get_mode("c"));
#endif

    cleanup();
    PASS();
}





SUITE(cp_preserve_mode_suite) {
    // RUN_TEST(test_cp_preserve_mode);
}
DEFINE_TEST_MAIN(cp_preserve_mode_suite)
