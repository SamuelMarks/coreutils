#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf d e loop file");
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

TEST test_cp_dir_rm_dest(void) {
    cleanup();

    MKDIR("d");
    MKDIR("e");

    // Do it once with no destination...
    ASSERT_EQ(0, exec_capture("cp -R --remove-destination d e", NULL, 0));

    // ...and again, with an existing destination.
    ASSERT_EQ(0, exec_capture("cp -R --remove-destination d e", NULL, 0));

#ifndef _WIN32
    // verify no ELOOP which was the case in <= 8.29
    if (symlink("loop", "loop") == 0) {
        create_file("file");
        ASSERT_EQ(0, exec_capture("cp --remove-destination file loop", NULL, 0));
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_dir_rm_dest_suite) {
    RUN_TEST(test_cp_dir_rm_dest);
}
DEFINE_TEST_MAIN(cp_dir_rm_dest_suite)
