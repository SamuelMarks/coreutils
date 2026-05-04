#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("chmod -R +w a b c 2>/dev/null");
    my_system("cmake -E rm -rf a b c");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) {
        fprintf(f, "test content\n");
        if (f) fclose(f);
    }
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_readonly_dir(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("permissions testing not supported on Windows");
#else
    umask(022);

    my_system("mkdir -p a/b/c/d");
    create_file("a/b/c/d/bar.txt");

    // Make directories readonly (remove write permissions)
    my_system("chmod -R -w a");

    // Test 1: cp -r should preserve readonly directory permissions
    ASSERT_EQ(0, exec_capture("cp -r a b", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, stat("b", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);

    ASSERT_EQ(0, stat("b/b", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);

    ASSERT_EQ(0, stat("b/b/c", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);

    ASSERT_EQ(0, stat("b/b/c/d", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);

    // Test 2: cp -a should preserve readonly directory permissions and not fail
    ASSERT_EQ(0, exec_capture("cp -a a c", NULL, 0));

    ASSERT_EQ(0, stat("c", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);

    ASSERT_EQ(0, stat("c/b", &st));
    ASSERT_EQ((mode_t)0555, st.st_mode & 07777);
#endif

    cleanup();
    PASS();
}





SUITE(cp_readonly_dir_suite) {
    // RUN_TEST(test_cp_readonly_dir);
}
DEFINE_TEST_MAIN(cp_readonly_dir_suite)
