#include "test_helper.h"
#include <locale.h>

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_unexpand_basic(void) {
    FILE *f = fopen("test_unexpand.txt", "w");
    fprintf(f, "a       b\n"); /* 7 spaces -> aligns at 8 with 'a', so it becomes tab */
    if (f) fclose(f);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("unexpand -a test_unexpand.txt", out, sizeof(out)));
    
    ASSERT(strstr(out, "a\tb") != NULL);
    
    remove("test_unexpand.txt");
    PASS();
}

TEST test_unexpand_bounded_memory(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("unexpand /dev/null", out, sizeof(out)));
    PASS();
}

TEST test_unexpand_pl(void) {
    char out[512];
    FILE *f = fopen("in", "w");
    fprintf(f, "        y\n"); // 8 spaces
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("unexpand -a in", out, sizeof(out)));
    ASSERT(strstr(out, "\ty\n") != NULL || strstr(out, "\ty\r\n") != NULL);
    
    remove("in");
    PASS();
}

TEST test_unexpand_mb(void) {
    SKIPm("Skipped natively");
    PASS();
}
TEST test_unexpand_unexpand(void) {
    char out[1024];
#ifndef _WIN32
    // infloop tests
    FILE *f = fopen("in_un", "w"); fprintf(f, " \t\t .\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -t 1,2 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("\t\t\t .\n", out);

    f = fopen("in_un", "w"); fprintf(f, "    \t\t \n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -t 4,5 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("\t\t\t \n", out);

    // blanks
    f = fopen("in_un", "w"); fprintf(f, " b  c   d\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -t 1 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("\tb\t\tc\t\t\td\n", out);
    
    // posix
    f = fopen("in_un", "w"); fprintf(f, "1234567   \t1\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -a in_un", out, sizeof(out)));
    ASSERT_STR_EQ("1234567\t\t1\n", out);

    f = fopen("in_un", "w"); fprintf(f, "1234567  1\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -a in_un", out, sizeof(out)));
    ASSERT_STR_EQ("1234567\t 1\n", out);
    
    // bs
    f = fopen("in_un", "w"); fprintf(f, "aa\b   c\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -a -t4 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("aa\b\tc\n", out);
    
    // u1..8
    f = fopen("in_un", "w"); fprintf(f, "a  b  c"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -a -3 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("a\tb\tc", out);

    f = fopen("in_un", "w"); fprintf(f, "a       b"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -a -8 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("a\tb", out);
    
    f = fopen("in_un", "w"); fprintf(f, "   a   b"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("unexpand -3 in_un", out, sizeof(out)));
    ASSERT_STR_EQ("\ta   b", out);

    remove("in_un");
#endif
    PASS();
}





SUITE(unexpand_suite) {
    RUN_TEST(test_unexpand_basic);
    RUN_TEST(test_unexpand_bounded_memory);
    RUN_TEST(test_unexpand_pl);
    RUN_TEST(test_unexpand_mb);
    RUN_TEST(test_unexpand_unexpand);
}
DEFINE_TEST_MAIN(unexpand_suite)
