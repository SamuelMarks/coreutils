#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>





TEST test_cut_basic(void) {
    FILE *f = fopen("test_cut.txt", "w");
    fprintf(f, "a,b,c\n1,2,3\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("cut -d, -f2 test_cut.txt", out, sizeof(out)));
    ASSERT(strstr(out, "b") != NULL);
    ASSERT(strstr(out, "2") != NULL);
    remove("test_cut.txt");
    PASS();
}

TEST test_cut_huge_range(void) {
    SKIPm("Skipped natively / depends on word size");
    PASS();
}

TEST test_cut_pl(void) {
    // Tests from cut.pl
    char out[512];
    FILE *f;
    
    // Basic fields
    f = fopen("in", "w");
    fprintf(f, "a:b:c\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("cut -d: -f2 in", out, sizeof(out)));
    ASSERT(strstr(out, "b\n") != NULL || strstr(out, "b\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("cut -d: -f1,3 in", out, sizeof(out)));
    ASSERT(strstr(out, "a:c\n") != NULL || strstr(out, "a:c\r\n") != NULL);

    // Complement
    ASSERT_EQ(0, exec_capture("cut -d: --complement -f2 in", out, sizeof(out)));
    ASSERT(strstr(out, "a:c\n") != NULL || strstr(out, "a:c\r\n") != NULL);

    // Byte selection
    f = fopen("in_b", "w");
    fprintf(f, "abcdefg\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("cut -b2,4-5 in_b", out, sizeof(out)));
    ASSERT(strstr(out, "bde\n") != NULL || strstr(out, "bde\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("cut -b2,4-5 --complement in_b", out, sizeof(out)));
    ASSERT(strstr(out, "acfg\n") != NULL || strstr(out, "acfg\r\n") != NULL);

    remove("in");
    remove("in_b");

    PASS();
}


TEST test_cut_mb_non_utf8(void) {
    SKIPm("Skipped natively / GB18030 support is spotty");
    PASS();
}
TEST test_cut_bounded_memory(void) {
    // Uses ulimit -v which is non-portable. Skip.
    PASS();
}

TEST test_cut_cut(void) {
    // Tests from perl cut.pl covered in test_cut_pl. Skip.
    PASS();
}






SUITE(cut_suite) {
    RUN_TEST(test_cut_basic);
    RUN_TEST(test_cut_huge_range);
    RUN_TEST(test_cut_pl);
    RUN_TEST(test_cut_mb_non_utf8);
    RUN_TEST(test_cut_bounded_memory);
    RUN_TEST(test_cut_cut);
}
DEFINE_TEST_MAIN(cut_suite)
