#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf dir1 dir2");
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

TEST test_cp_dir_slash(void) {
    cleanup();

    MKDIR("dir1");
    MKDIR("dir2");
    create_file("dir1/file");

    // cp -R dir1/ dir2
    ASSERT_EQ(0, exec_capture("cp -R dir1/ dir2", NULL, 0));

    struct stat st;
    
    // dir2/file should not exist
    ASSERT_EQ(-1, stat("dir2/file", &st));
    
    // dir2/dir1/file should exist
    ASSERT_EQ(0, stat("dir2/dir1/file", &st));
    
    // dir1/file should exist
    ASSERT_EQ(0, stat("dir1/file", &st));

    cleanup();
    PASS();
}





SUITE(cp_dir_slash_suite) {
    RUN_TEST(test_cp_dir_slash);
}
DEFINE_TEST_MAIN(cp_dir_slash_suite)
