#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>

#define ASSERT_EXIT(cmd, expected) \
    do { \
        int _r = exec_capture(cmd, NULL, 0); \
        ASSERT_EQ(expected, WEXITSTATUS(_r)); \
    } while (0)

TEST test_cmd_basic(void) {
    ASSERT_EXIT("test 1 -eq 1", 0);
    ASSERT_EXIT("test 1 -eq 2", 1);
    
    // empty string
    ASSERT_EXIT("test", 1);
    ASSERT_EXIT("test -z ''", 0);
    ASSERT_EXIT("test any-string", 0);
    ASSERT_EXIT("test -n any-string", 0);
    ASSERT_EXIT("test ''", 1);
    ASSERT_EXIT("test -", 0);
    ASSERT_EXIT("test --", 0);
    ASSERT_EXIT("test -0", 0);
    ASSERT_EXIT("test -f", 0);

    // string eq
    ASSERT_EXIT("test t = t", 0);
    ASSERT_EXIT("test t = f", 1);
    ASSERT_EXIT("test t == t", 0);
    ASSERT_EXIT("test t == f", 1);
    ASSERT_EXIT("test ! = !", 0);
    ASSERT_EXIT("test = = =", 0);
    ASSERT_EXIT("test '(' = '('", 0);
    ASSERT_EXIT("test '(' = ')'", 1);

    // string ne
    ASSERT_EXIT("test t != t", 1);
    ASSERT_EXIT("test t != f", 0);
    ASSERT_EXIT("test ! != !", 1);
    ASSERT_EXIT("test = != =", 1);
    ASSERT_EXIT("test '(' != '('", 1);
    ASSERT_EXIT("test '(' != ')'", 0);

    // and
    ASSERT_EXIT("test t -a t", 0);
    ASSERT_EXIT("test '' -a t", 1);
    ASSERT_EXIT("test t -a ''", 1);
    ASSERT_EXIT("test '' -a ''", 1);

    // or
    ASSERT_EXIT("test t -o t", 0);
    ASSERT_EXIT("test '' -o t", 0);
    ASSERT_EXIT("test t -o ''", 0);
    ASSERT_EXIT("test '' -o ''", 1);

    // int eq
    ASSERT_EXIT("test 9 -eq 9", 0);
    ASSERT_EXIT("test 0 -eq 0", 0);
    ASSERT_EXIT("test 0 -eq 00", 0);
    ASSERT_EXIT("test 8 -eq 9", 1);
    ASSERT_EXIT("test 1 -eq 0", 1);
    ASSERT_EXIT("test 0 -eq ' 0 '", 0);

    // int gt
    ASSERT_EXIT("test 5 -gt 5", 1);
    ASSERT_EXIT("test 5 -gt 4", 0);
    ASSERT_EXIT("test 4 -gt 5", 1);
    ASSERT_EXIT("test -1 -gt -2", 0);

    // int lt
    ASSERT_EXIT("test 5 -lt 5", 1);
    ASSERT_EXIT("test 5 -lt 4", 1);
    ASSERT_EXIT("test 4 -lt 5", 0);
    ASSERT_EXIT("test -1 -lt -2", 1);
    
    // int invalid
    ASSERT_EXIT("test 0x0 -eq 00 2>/dev/null", 2);

    // parentheses
    ASSERT_EXIT("test '(' '' ')'", 1);
    ASSERT_EXIT("test '(' '(' ')'", 0);
    ASSERT_EXIT("test '(' ')' ')'", 0);
    ASSERT_EXIT("test '(' ! ')'", 0);
    ASSERT_EXIT("test '(' -a ')'", 0);
    
    // collation / lexicographic
    ASSERT_EXIT("test a '<' b", 0);
    ASSERT_EXIT("test a '<' a", 1);
    ASSERT_EXIT("test b '<' a", 1);
    
    ASSERT_EXIT("test b '>' a", 0);
    ASSERT_EXIT("test a '>' a", 1);
    ASSERT_EXIT("test a '>' b", 1);

    PASS();
}

TEST test_cmd_diagnostics(void) {
    char out[1024];
    
    // test-diag.pl
    int r = exec_capture("test -o arg 2>&1", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));
    
    // some others that test might output
    r = exec_capture("test 1 -eq a 2>&1", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));

    PASS();
}

TEST test_cmd_N(void) {
    exec_capture("rm -f test_N_file", NULL, 0);
    
    exec_capture("touch test_N_file", NULL, 0);
    
    // Caching/resolutions issues might happen, so just touch with specific times.
    // Set atime to 2 days ago: test -N returns 0 (since mtime is newer)
    exec_capture("touch -a -d '12:00 today -2 days' test_N_file", NULL, 0);
    ASSERT_EXIT("test -N test_N_file", 0);
    
    // Set mtime to 4 days ago: test -N returns 1 (since atime is newer)
    exec_capture("touch -m -d '12:00 today -4 days' test_N_file", NULL, 0);
    ASSERT_EXIT("test -N test_N_file", 1);
    
    PASS();
}

SUITE(test_cmd_suite) {
    RUN_TEST(test_cmd_basic);
    RUN_TEST(test_cmd_diagnostics);
    RUN_TEST(test_cmd_N);
}
DEFINE_TEST_MAIN(test_cmd_suite)
