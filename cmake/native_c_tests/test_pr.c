#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>

TEST test_pr_basic(void) {
    char out[512];
    FILE *f = fopen("test_pr.txt", "w");
    fprintf(f, "hello\nworld\n");
    if (f) fclose(f);
    
    int _r = exec_capture("pr -t -n test_pr.txt", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(_r));
    
    remove("test_pr.txt");
    PASS();
}

TEST test_pr_sh(void) {
    SKIPm("Skipped: no tests/pr/*.sh script other than bounded-memory");
    PASS();
}
TEST test_pr_pl(void) {
    char out[1024];
#ifndef _WIN32
    // test pr -t
    FILE *f = fopen("in_pr", "w"); fprintf(f, "a\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("pr -t in_pr", out, sizeof(out)));
    ASSERT_STR_EQ("a\n", out);
    
    // test pr -n
    ASSERT_EQ(0, exec_capture("pr -t -n in_pr", out, sizeof(out)));
    ASSERT(strstr(out, "1\t") != NULL);
    
    remove("in_pr");
#endif
    PASS();
}

TEST test_pr_bounded_memory(void) {
    char out[1024];
#ifndef _WIN32
    int _r = exec_capture("timeout 0.5 pr < /dev/zero >/dev/null 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(124, WEXITSTATUS(_r));
#endif
    PASS();
}

SUITE(pr_suite) { 
    RUN_TEST(test_pr_basic); 
    RUN_TEST(test_pr_sh); 
    RUN_TEST(test_pr_pl); 
    RUN_TEST(test_pr_bounded_memory);
}
DEFINE_TEST_MAIN(pr_suite)

// tests/pr/pr-tests.pl pr_tests
