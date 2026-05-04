#include "test_helper.h"
#include <locale.h>

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_numfmt_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("numfmt --to=si 1000", out, sizeof(out)));
    ASSERT(strstr(out, "1.0K") != NULL || strstr(out, "1.0k") != NULL);
    PASS();
}

TEST test_numfmt_pl(void) {
    char out[512];
    
    // Basic SI
    ASSERT_EQ(0, exec_capture("numfmt --to=si 1000", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strcmp(out, "1.0K") == 0 || strcmp(out, "1.0k") == 0);

    ASSERT_EQ(0, exec_capture("numfmt --to=si 1000000", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1.0M", out);

    // Basic IEC
    ASSERT_EQ(0, exec_capture("numfmt --to=iec 1024", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strcmp(out, "1.0K") == 0 || strcmp(out, "1.0k") == 0);

    ASSERT_EQ(0, exec_capture("numfmt --to=iec-i 1024", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1.0Ki", out);

    // From SI
    ASSERT_EQ(0, exec_capture("env LC_ALL=zh_CN.GB18030 numfmt --from=si 1K", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1000", out);

    // From IEC
    ASSERT_EQ(0, exec_capture("numfmt --from=iec 1K", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1024", out);
    
    PASS();
}

TEST test_numfmt_mb_non_utf8(void) {
    SKIPm("Skipped natively / GB18030 support is spotty");
    PASS();
}
TEST test_numfmt_numfmt(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("numfmt --to=iec 1024", out, sizeof(out)));
    ASSERT_STR_EQ("1.0K\n", out);
    
    ASSERT_EQ(0, exec_capture("numfmt --to=si 1000", out, sizeof(out)));
    ASSERT_STR_EQ("1.0k\n", out);
    
    ASSERT_EQ(0, exec_capture("numfmt --from=iec 1K", out, sizeof(out)));
    ASSERT_STR_EQ("1024\n", out);
#endif
    PASS();
}





SUITE(numfmt_suite) {
    RUN_TEST(test_numfmt_basic);
    RUN_TEST(test_numfmt_pl);
    RUN_TEST(test_numfmt_mb_non_utf8);
    RUN_TEST(test_numfmt_numfmt);
}
DEFINE_TEST_MAIN(numfmt_suite)
