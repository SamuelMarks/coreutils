#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf src-dir dest-dir f slink");
}

TEST test_cp_HL(void) {
    cleanup();

#ifdef _WIN32
    mkdir("src-dir");
    mkdir("dest-dir");
#else
    mkdir("src-dir", 0700);
    mkdir("dest-dir", 0700);
#endif

    FILE *f_file = fopen("f", "w");
    if (f_file) {
        fprintf(f_file, "f\n");
        fclose(f_file);
    }

    if (symlink("f", "slink") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }

    if (symlink("no-such-file", "src-dir/slink") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }

    ASSERT_EQ(0, exec_capture("cp -H -R slink src-dir dest-dir", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, stat("src-dir", &st));
    ASSERT(S_ISDIR(st.st_mode));
    
    ASSERT_EQ(0, stat("dest-dir/src-dir", &st));
    ASSERT(S_ISDIR(st.st_mode));

    // dest-dir/slink is copied as regular file
    ASSERT_EQ(0, lstat("dest-dir/slink", &st));
    ASSERT(S_ISREG(st.st_mode));

    // dest-dir/src-dir/slink is copied as symlink
    ASSERT_EQ(0, lstat("dest-dir/src-dir/slink", &st));
    ASSERT(S_ISLNK(st.st_mode));
    
    // It is dangling
    ASSERT_NEQ(0, stat("dest-dir/src-dir/slink", &st));

    cleanup();
    PASS();
}





SUITE(cp_HL_suite) {
    RUN_TEST(test_cp_HL);
}
DEFINE_TEST_MAIN(cp_HL_suite)
