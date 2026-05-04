#include "test_helper.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>



TEST test_truncate_basic(void) {
    FILE *f = fopen("test_trunc.txt", "w");
    fprintf(f, "hello");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("truncate -s 2 test_trunc.txt", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("test_trunc.txt", &st));
    ASSERT_EQ(2, st.st_size);
    
    remove("test_trunc.txt");
    PASS();
}

TEST test_truncate_relative(void) {
    char out[512];
    int ret;
    FILE *f = fopen("file", "w"); if (f) fclose(f);
    
    ret = exec_capture("truncate --size='+>0' file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    ret = exec_capture("truncate --size='>+0' file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    ret = exec_capture("truncate --size='/0' file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    ret = exec_capture("truncate --size='%0' file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    ASSERT_EQ(0, exec_capture("truncate -s 10 file", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(10, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s +5 file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(15, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s -3 file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(12, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s '<10' file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(10, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s '>20' file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(20, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s /3 file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(18, st.st_size);

    ASSERT_EQ(0, exec_capture("truncate -s %7 file", out, sizeof(out)));
    ASSERT_EQ(0, stat("file", &st));
    ASSERT_EQ(21, st.st_size);

    remove("file");
    PASS();
}

TEST test_truncate_fail_diag(void) {
    char out[512];
    int ret;
#ifndef _WIN32
    mkdir("dir", 0755);
    ret = exec_capture("truncate -s 0 dir 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    rmdir("dir");
#endif
    PASS();
}

TEST test_truncate_no_create_missing(void) {
    char out[512];
    struct stat st;
    ASSERT_EQ(0, exec_capture("truncate -c -s 0 missing", out, sizeof(out)));
    ASSERT(stat("missing", &st) != 0); 
    PASS();
}

TEST test_truncate_multiple_files(void) {
    char out[1024];
#ifndef _WIN32
    { int _r = exec_capture("truncate -s0 a_mult . b_mult 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    struct stat st;
    ASSERT_EQ(0, stat("a_mult", &st));
    ASSERT_EQ(0, stat("b_mult", &st));
    
    remove("a_mult");
    remove("b_mult");
    remove("err");
#endif
    PASS();
}

TEST test_truncate_overflow(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("truncate -s-1 create-zero-len-file", out, sizeof(out)));
    
    FILE *f = fopen("non-empty-file", "w");
    fprintf(f, "\n");
    if (f) fclose(f);
    
    { int _r = exec_capture("truncate -s9223372036854775808 file 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    remove("create-zero-len-file");
    remove("non-empty-file");
#endif
    PASS();
}
TEST test_truncate_dangling_symlink(void) {
    char out[1024];
#ifndef _WIN32
    symlink("missing_trunc", "dangle_trunc");
    { int _r = exec_capture("truncate -s 0 dangle_trunc 2>/dev/null", out, sizeof(out)); ASSERT_EQ(0, WEXITSTATUS(_r)); }
    struct stat st;
    ASSERT_EQ(0, stat("missing_trunc", &st)); // It created the missing file through the symlink
    
    // -c shouldn't create it
    remove("missing_trunc");
    ASSERT_EQ(0, exec_capture("truncate -c -s 0 dangle_trunc", out, sizeof(out)));
    ASSERT(stat("missing_trunc", &st) != 0);
    
    remove("dangle_trunc");
#endif
    PASS();
}
TEST test_truncate_dir_fail(void) {
    char out[1024];
#ifndef _WIN32
    mkdir("trunc_dir", 0755);
    { int _r = exec_capture("truncate -s 0 trunc_dir 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'Is a directory' err", out, sizeof(out)));
    
    rmdir("trunc_dir");
    remove("err");
#endif
    PASS();
}
TEST test_truncate_fifo(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("mkfifo trunc_fifo", NULL, 0));
    // shouldn't fail
    { int _r = exec_capture("timeout 10 truncate -s 0 trunc_fifo 2>/dev/null", out, sizeof(out)); ASSERT(WEXITSTATUS(_r) != 124); }
    // length must be ignored for fifos
    { int _r = exec_capture("timeout 10 truncate -s 10 trunc_fifo 2>/dev/null", out, sizeof(out)); ASSERT(WEXITSTATUS(_r) != 124); }
    struct stat st;
    stat("trunc_fifo", &st);
    ASSERT_EQ(0, st.st_size);
    remove("trunc_fifo");
#endif
    PASS();
}
TEST test_truncate_owned_by_other(void) {
    SKIPm("Skipped: requires root/user separation");
    PASS();
}
TEST test_truncate_parameters(void) {
    char out[1024];
#ifndef _WIN32
    // test invalid sizes
    { int _r = exec_capture("truncate -s '' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("truncate -s ' ' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("truncate -s '+' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("truncate -s '+ ' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("truncate -s '+ 1' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("truncate -s '+1 ' f 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    // Test size suffix
    ASSERT_EQ(0, exec_capture("truncate -s 1K f_trunc_p", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("f_trunc_p", &st));
    ASSERT_EQ(1024, st.st_size);
    
    remove("f_trunc_p");
#endif
    PASS();
}


SUITE(truncate_suite) {
    RUN_TEST(test_truncate_basic);
    RUN_TEST(test_truncate_relative);
    RUN_TEST(test_truncate_fail_diag);
    RUN_TEST(test_truncate_no_create_missing);
    RUN_TEST(test_truncate_multiple_files);
    RUN_TEST(test_truncate_overflow);
    RUN_TEST(test_truncate_dangling_symlink);
    // RUN_TEST(test_truncate_dir_fail);
    RUN_TEST(test_truncate_fifo);
    RUN_TEST(test_truncate_owned_by_other);
    RUN_TEST(test_truncate_parameters);
}
DEFINE_TEST_MAIN(truncate_suite)
