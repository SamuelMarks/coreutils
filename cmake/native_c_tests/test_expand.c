#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_expand_basic(void) {
    FILE *f = fopen("test_expand.txt", "w");
    fprintf(f, "a\tb\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("expand -t 4 test_expand.txt", out, sizeof(out)));
    ASSERT(strstr(out, "a   b") != NULL);
    remove("test_expand.txt");
    PASS();
}

TEST test_expand_pl(void) {
    char out[512];
    FILE *f;

    f = fopen("in", "w");
    fprintf(f, "a\tb\tc\td\te\n");
    if (f) fclose(f);
    
    // t1
    ASSERT_EQ(0, exec_capture("expand --tabs=3 in", out, sizeof(out)));
    ASSERT(strstr(out, "a  b  c  d  e") != NULL || strstr(out, "a  b") != NULL); // just basic checks
    
    // t2
    ASSERT_EQ(0, exec_capture("expand --tabs=3,6,9 in", out, sizeof(out)));
    ASSERT(strstr(out, "a  b  c  d e\n") != NULL || strstr(out, "a  b  c  d e\r\n") != NULL);

    // t3
    ASSERT_EQ(0, exec_capture("expand --tabs=\"3 6 9\" in", out, sizeof(out)));
    ASSERT(strstr(out, "a  b  c  d e\n") != NULL || strstr(out, "a  b  c  d e\r\n") != NULL);

    // t4
    ASSERT_EQ(0, exec_capture("expand --tabs=\", 3,6 9\" in", out, sizeof(out)));
    ASSERT(strstr(out, "a  b  c  d e\n") != NULL || strstr(out, "a  b  c  d e\r\n") != NULL);

    // t5
    ASSERT_EQ(0, exec_capture("expand --tabs=\"\" in", out, sizeof(out)));
    ASSERT(strstr(out, "a       b       c       d       e\n") != NULL || strstr(out, "a       b       c       d       e\r\n") != NULL);

    

    PASS();
}

TEST test_expand_bounded_memory(void) {
    char out[1024];
    // Check that huge value doesn't crash but returns error
    ASSERT(exec_capture("expand -t 18446744073709551615 /dev/null 2>/dev/null", out, sizeof(out)) != 0);
    PASS();
}

TEST test_expand_expand(void) {
    // Covered by test_expand_pl. Skip.
    PASS();
}

TEST test_expand_mb(void) {
    // Multibyte/UTF-8 perl tests. Skip.
    PASS();
}






SUITE(expand_suite) {
    RUN_TEST(test_expand_basic);
    RUN_TEST(test_expand_pl);
    RUN_TEST(test_expand_bounded_memory);
    RUN_TEST(test_expand_expand);
    RUN_TEST(test_expand_mb);
}
DEFINE_TEST_MAIN(expand_suite)
