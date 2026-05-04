#include "test_helper.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef _WIN32
#include <direct.h>
#endif



#ifdef _WIN32
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0777)
#endif

TEST test_rmdir_basic(void) {
    MKDIR("test_rmdir_dir");
    ASSERT_EQ(0, exec_capture("rmdir test_rmdir_dir", NULL, 0));
    struct stat st;
    ASSERT(stat("test_rmdir_dir", &st) != 0);
    PASS();
}

TEST test_rmdir_fail_perm(void) {
    char out[512];
    int ret;
    MKDIR("d");
    MKDIR("d/e");
    MKDIR("d/e/f");
#ifndef _WIN32
    chmod("d", 0555); // a-w
    ret = exec_capture("rmdir -p d d/e/f 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    chmod("d", 0755);
#endif
    rmdir("d/e/f");
    rmdir("d/e");
    rmdir("d");
    PASS();
}

TEST test_rmdir_ignore(void) {
    SKIPm("Skipped natively");
    PASS();
}

TEST test_rmdir_t_slash(void) {
    char out[512];
    int ret;
    MKDIR("d");
    ASSERT_EQ(0, exec_capture("rmdir d/", out, sizeof(out)));
    
    FILE *f = fopen("f", "w");
    if (f) fclose(f);
    ret = exec_capture("rmdir f/ 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    unlink("f");
    PASS();
}

TEST test_rmdir_symlink(void) {
    char out[512];
    int ret;
    MKDIR("d");
    
#ifndef _WIN32
    symlink("d", "sym");
    ret = exec_capture("rmdir sym 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    unlink("sym");
#endif

    rmdir("d");
    PASS();
}

TEST test_rmdir_symlink_errors(void) {
    SKIPm("Skipped natively");
    PASS();
}





SUITE(rmdir_suite) {
    RUN_TEST(test_rmdir_basic);
    RUN_TEST(test_rmdir_fail_perm);
    RUN_TEST(test_rmdir_ignore);
    RUN_TEST(test_rmdir_t_slash);
    RUN_TEST(test_rmdir_symlink);
    RUN_TEST(test_rmdir_symlink_errors);
}
DEFINE_TEST_MAIN(rmdir_suite)
