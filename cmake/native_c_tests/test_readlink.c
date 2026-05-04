#include "test_helper.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>




TEST test_readlink_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("readlink --version", out, sizeof(out)));
    ASSERT(strstr(out, "readlink") != NULL);
    PASS();
}

TEST test_readlink_basic_links(void) {
    char out[512];
    int ret;
    
#ifndef _WIN32
    mkdir("subdir", 0755);
#else
    mkdir("subdir");
#endif
    FILE *f = fopen("regfile", "w"); if (f) fclose(f);
    symlink("regfile", "link1");
    symlink("missing", "link2");
    
    ASSERT_EQ(0, exec_capture("readlink link1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("regfile", out);
    
    ASSERT_EQ(0, exec_capture("readlink link2", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("missing", out);
    
    ret = exec_capture("readlink subdir", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT_EQ(0, out[0]); 
    
    ret = exec_capture("readlink regfile", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT_EQ(0, out[0]);
    
    ret = exec_capture("readlink missing", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT_EQ(0, out[0]);
    
    unlink("link1");
    unlink("link2");
    unlink("regfile");
    rmdir("subdir");
    PASS();
}

TEST test_readlink_multi(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("f1", "w"); if (f) fclose(f);
    f = fopen("f2", "w"); if (f) fclose(f);
    symlink("f1", "l1");
    symlink("f2", "l2");
    
    ASSERT_EQ(0, exec_capture("readlink l1 l2", out, sizeof(out)));
    ASSERT(strstr(out, "f1\n") != NULL || strstr(out, "f1\r\n") != NULL);
    ASSERT(strstr(out, "f2\n") != NULL || strstr(out, "f2\r\n") != NULL);
    
    ret = exec_capture("readlink l1 missing l2", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT(strstr(out, "f1\n") != NULL || strstr(out, "f1\r\n") != NULL);
    ASSERT(strstr(out, "f2\n") != NULL || strstr(out, "f2\r\n") != NULL);
    
    unlink("l1"); unlink("l2");
    unlink("f1"); unlink("f2");
    PASS();
}

TEST test_readlink_can(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p rl_tmp/subdir");
    FILE *f = fopen("rl_tmp/regfile", "w"); if (f) fclose(f);
    chdir("rl_tmp");
    symlink("regfile", "link1");
    symlink("subdir", "link2");
    symlink("missing", "link3");
    symlink("subdir/missing", "link4");
    chdir("..");
    
    // -e (existing)
    ASSERT_EQ(0, exec_capture("readlink -e rl_tmp/link1", out, sizeof(out)));
    { int _r = exec_capture("readlink -e rl_tmp/link3 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    // -f (missing leaf is ok)
    ASSERT_EQ(0, exec_capture("readlink -f rl_tmp/link3", out, sizeof(out)));
    { int _r = exec_capture("readlink -f rl_tmp/link3/more 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    // -m (missing is totally ok)
    ASSERT_EQ(0, exec_capture("readlink -m rl_tmp/link3/more", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("readlink -m rl_tmp/link4/more/more2", out, sizeof(out)));
    
    my_system("rm -rf rl_tmp");
#endif
    PASS();
}

TEST test_readlink_fp_loop(void) {
    char out[1024];
#ifndef _WIN32
    // create fake loop setup
    my_system("mkdir -p rl_lp/d");
    chdir("rl_lp");
    symlink("s", "p");
    symlink("d", "s");
    FILE *f = fopen("d/2", "w"); if (f) fclose(f);
    symlink("../s/2", "d/1");
    
    // valid path through the symlinks
    ASSERT_EQ(0, exec_capture("readlink -v -e p/1", out, sizeof(out)));
    
    // construct real loop
    remove("d/2"); symlink("../s/1", "d/2");
    { int _r = exec_capture("readlink -v -e p/1 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    chdir("..");
    my_system("rm -rf rl_lp");
#endif
    PASS();
}
TEST test_readlink_can_f(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("readlink -f /", out, sizeof(out)));
    ASSERT_STR_EQ("/\n", out);
#endif
    PASS();
}
TEST test_readlink_can_m(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("readlink -m /missing", out, sizeof(out)));
    ASSERT_STR_EQ("/missing\n", out);
#endif
    PASS();
}
TEST test_readlink_posix(void) {
    char out[1024];
#ifndef _WIN32
    my_system("touch rl_file");
    my_system("ln -s rl_file rl_link1");

    int r = exec_capture("env POSIXLY_CORRECT=1 readlink rl_file 2>&1", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    ASSERT(strstr(out, "Invalid argument") != NULL || strstr(out, "invalid argument") != NULL);

    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 readlink -f rl_file", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 readlink -e rl_file", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 readlink -m rl_file", out, sizeof(out)));

    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 readlink rl_link1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("rl_file", out);

    my_system("rm -f rl_file rl_link1");
#endif
    PASS();
}

TEST test_readlink_root(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("readlink -e /", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("/", out);
#endif
    PASS();
}

TEST test_readlink_rl_1(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir rl_subdir");
    my_system("touch rl_regfile");
    my_system("ln -s rl_regfile rl_link1");
    my_system("ln -s rl_missing rl_link2");

    ASSERT_EQ(0, exec_capture("readlink rl_link1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("rl_regfile", out);

    ASSERT_EQ(0, exec_capture("readlink rl_link2", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("rl_missing", out);

    { int _r = exec_capture("readlink rl_subdir", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, strlen(out));

    { int _r = exec_capture("readlink rl_regfile", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, strlen(out));

    { int _r = exec_capture("readlink rl_missing", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, strlen(out));

    my_system("rm -rf rl_subdir rl_regfile rl_link1 rl_link2");
#endif
    PASS();
}
TEST test_readlink_can_e(void) {
    char out[1024];
#ifndef _WIN32
    { int _r = exec_capture("readlink -e /missing 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("readlink -e /", out, sizeof(out)));
    ASSERT_STR_EQ("/\n", out);
#endif
    PASS();
}




SUITE(readlink_suite) {
    RUN_TEST(test_readlink_basic);
    RUN_TEST(test_readlink_basic_links);
    RUN_TEST(test_readlink_multi);
    RUN_TEST(test_readlink_can);
    RUN_TEST(test_readlink_fp_loop);
    RUN_TEST(test_readlink_can_f);
    RUN_TEST(test_readlink_can_m);
    RUN_TEST(test_readlink_posix);
    RUN_TEST(test_readlink_root);
    RUN_TEST(test_readlink_rl_1);
    RUN_TEST(test_readlink_can_e);
}
DEFINE_TEST_MAIN(readlink_suite)
