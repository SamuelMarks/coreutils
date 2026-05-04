#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf src_dir dst_dir");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path, mode) mkdir(path, mode)
#endif

TEST test_cp_existing_perm_dir(void) {
    cleanup();

#ifndef _WIN32
    umask(002);

    MKDIR("src_dir", 0700);
    MKDIR("src_dir/dir", 0775); // u=rwx,g=rwx,o=rx
    
    MKDIR("dst_dir", 0700);
    MKDIR("dst_dir/dir", 0700); // u=rwx,g=,o=

    char out[1024];
    int res = exec_capture("cp -r src_dir/. dst_dir/", out, sizeof(out));
    if (res != 0) {
        printf("cp output: %s\n", out);
    }
    ASSERT_EQ(0, res);

    struct stat st;
    ASSERT_EQ(0, stat("dst_dir/dir", &st));
    
    mode_t expected = S_IRWXU; // 0700
    ASSERT_EQ(expected, st.st_mode & 07777);
#else
    SKIPm("permissions testing not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_existing_perm_dir_suite) {
    RUN_TEST(test_cp_existing_perm_dir);
}
DEFINE_TEST_MAIN(cp_existing_perm_dir_suite)
