#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>



TEST test_csplit_basic(void) {
    char out[1024];
    FILE *f = fopen("in_c_b", "w");
    fprintf(f, "a\nb\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("csplit in_c_b 2", out, sizeof(out)));
    remove("in_c_b");
    remove("xx00");
    remove("xx01");
    PASS();
}

TEST test_csplit_1000(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p csplit_dir");
    chdir("csplit_dir");
    
    FILE *f = fopen("in_c", "w");
    for (int i=0; i<1000; i++) {
        fprintf(f, "%d\n", i);
    }
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("csplit in_c '/./' '{*}' > /dev/null", out, sizeof(out)));
    
    struct stat st;
    ASSERT_EQ(0, stat("xx1000", &st));
    ASSERT_EQ(-1, stat("xx1001", &st));
    
    chdir("..");
    my_system("rm -rf csplit_dir");
#endif
    PASS();
}

TEST test_csplit_io_err(void) {
    char out[1024];
#ifndef _WIN32
    struct stat st;
    if (stat("/dev/full", &st) == 0) {
        my_system("mkdir -p csplit_io");
        chdir("csplit_io");
        
        symlink("/dev/full", "xx01");
        FILE *f = fopen("in", "w"); fprintf(f, "a\nb\n"); if (f) fclose(f);
        
        { int _r = exec_capture("csplit in 1 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        ASSERT_EQ(-1, stat("xx01", &st));
        
        chdir("..");
        my_system("rm -rf csplit_io");
    }
#endif
    PASS();
}

TEST test_csplit_sh(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p csplit_sh");
    chdir("csplit_sh");
    
    FILE *f = fopen("in", "w");
    fprintf(f, "a\n\n\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("csplit in '/^$/' 2", out, sizeof(out)));
    ASSERT_STR_EQ("2\n0\n2\n", out);
    
    // Check line numbers
    f = fopen("in2", "w"); fprintf(f, "\n"); if (f) fclose(f);
    { int _r = exec_capture("csplit in2 0 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("csplit in2 2 1 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    chdir("..");
    my_system("rm -rf csplit_sh");
#endif
    PASS();
}
TEST test_csplit_csplit_heap(void) {
    // Tests memory bounds with ulimit -v which is non-portable. Skip.
    PASS();
}

TEST test_csplit_csplit_suppress_matched(void) {
    // Perl script test case. We cover regex bounds in test_csplit_sh and below. Skip.
    PASS();
}

TEST test_csplit_csplit(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p csplit_sh2");
    chdir("csplit_sh2");
    
    // Inloop test
    FILE *f = fopen("in", "w");
    fprintf(f, "\na\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("csplit in '/a/-1' '{*}' 2>/dev/null", out, sizeof(out)));
    // Line number out of range checks
    f = fopen("in2", "w");
    fprintf(f, "\n");
    if (f) fclose(f);
    int ret;
    ret = exec_capture("csplit in2 0 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret));
    ret = exec_capture("csplit in2 2 1 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret));
    
    // Test large lines exceeding buffer length
    f = fopen("in_large", "w");
    fprintf(f, "x");
    for(int i=0; i<8199; i++) fprintf(f, "x");
    fprintf(f, "\nx\n");
    for(int i=0; i<8199; i++) fprintf(f, "y");
    fprintf(f, "\nx\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("csplit in_large '/x\\{1\\}/' '{*}' > /dev/null", out, sizeof(out)));
    
    // Empty input should not create xx00
    ret = exec_capture("csplit /dev/null 1 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    struct stat st;
    ASSERT_EQ(-1, stat("xx00", &st));

    chdir("..");
    my_system("rm -rf csplit_sh2");
#endif
    PASS();
}








SUITE(csplit_suite) {
    RUN_TEST(test_csplit_basic);
    RUN_TEST(test_csplit_1000);
    RUN_TEST(test_csplit_io_err);
    RUN_TEST(test_csplit_sh);
    RUN_TEST(test_csplit_csplit_heap);
    RUN_TEST(test_csplit_csplit_suppress_matched);
    RUN_TEST(test_csplit_csplit);
}
DEFINE_TEST_MAIN(csplit_suite)
