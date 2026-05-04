#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_lbracket_basic(void) {
    ASSERT_EQ(0, exec_capture("lbracket 1 -eq 1 ]", NULL, 0));
    ASSERT(exec_capture("lbracket 1 -eq 2 ]", NULL, 0) != 0);
    PASS();
}



TEST test_lbracket_diag(void) {
    char out[512];
    int r = exec_capture("test -o arg 2>&1", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));
    ASSERT(strstr(out, "unary operator expected") != NULL);
    
    r = exec_capture("test -a arg 2>&1", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));
    ASSERT(strstr(out, "unary operator expected") != NULL);
    PASS();
}

TEST test_lbracket_file(void) {
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("touch test_file");
        chmod("test_file", 0);
        
        ASSERT_EQ(0, exec_capture("test -f test_file", NULL, 0));
        { int _r = exec_capture("test -f fail", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("test -r test_file", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("test -w test_file", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("test -x test_file", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        
        chmod("test_file", 0755);
        ASSERT_EQ(0, exec_capture("test -r test_file", NULL, 0));
        ASSERT_EQ(0, exec_capture("test -w test_file", NULL, 0));
        ASSERT_EQ(0, exec_capture("test -x test_file", NULL, 0));
        
        remove("test_file");
    }
#endif
    PASS();
}

TEST test_lbracket_N(void) {
    SKIPm("Skipped natively / timing granularity issues");
    PASS();
}

TEST test_lbracket_pl(void) {
    char out[512];
    
    ASSERT_EQ(0, exec_capture("test 1 = 1", NULL, 0));
    { int _r = exec_capture("test 1 = 2", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("test 1 -eq 1", NULL, 0));
    { int _r = exec_capture("test 1 -eq 2", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    ASSERT_EQ(0, exec_capture("test 2 -gt 1", NULL, 0));
    { int _r = exec_capture("test 1 -gt 2", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    ASSERT_EQ(0, exec_capture("test -n 'foo'", NULL, 0));
    { int _r = exec_capture("test -n ''", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    ASSERT_EQ(0, exec_capture("test -z ''", NULL, 0));
    { int _r = exec_capture("test -z 'foo'", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    PASS();
}

SUITE(lbracket_suite) {
    RUN_TEST(test_lbracket_basic);
    RUN_TEST(test_lbracket_diag);
    RUN_TEST(test_lbracket_file);
    RUN_TEST(test_lbracket_N);
    RUN_TEST(test_lbracket_pl);
}
DEFINE_TEST_MAIN(lbracket_suite)
