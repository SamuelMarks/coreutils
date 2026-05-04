#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>





TEST test_tr_basic(void) {
    char out[512];
    int ret;
    
    // Case classes
    ASSERT_EQ(0, exec_capture("echo 'abcdefghijklmnopqrstuvwxyz' | tr '[:lower:]' '0-9'", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("01234567899999999999999999", out);

    ASSERT_EQ(0, exec_capture("echo 'abcdefghijklmnopqrstuvwxyz' | tr '[:lower:][:lower:]' '[:upper:]0-9'", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("01234567899999999999999999", out); // wait, is this true? Yes from the script!

    // Validate alignment
    ret = exec_capture("tr 'A-Z[:lower:]' 'a-y[:upper:]' </dev/null 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    ASSERT_EQ(0, exec_capture("LC_ALL=C tr '[:upper:][:lower:]' 'a-z[:upper:]' < /dev/null", out, sizeof(out)));

    // PL basic tests
    ASSERT_EQ(0, exec_capture("echo abcd | tr abcd '[]*]'", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("]]]]", out);

    ASSERT_EQ(0, exec_capture("echo abc | tr abc '[%*]xyz'", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("xyz", out);

    ASSERT_EQ(0, exec_capture("echo abcde | tr -t abcd xy", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("xycde", out);

    ASSERT_EQ(0, exec_capture("echo abcde | tr abcd xy", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("xyyye", out);

    PASS();
}

TEST test_tr_tr_case_class(void) {
    char out[1024];
#ifndef _WIN32
    // Test that tr [:lower:] [:upper:] works
    ASSERT_EQ(0, exec_capture("echo 'hello' | tr '[:lower:]' '[:upper:]'", out, sizeof(out)));
    ASSERT_STR_EQ("HELLO\n", out);
#endif
    PASS();
}
TEST test_tr_tr(void) {
    char out[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("echo 'aabbcc' | tr -s 'b'", out, sizeof(out)));
    ASSERT_STR_EQ("aabcc\n", out);

    ASSERT_EQ(0, exec_capture("echo 'abc' | tr -d 'b'", out, sizeof(out)));
    ASSERT_STR_EQ("ac\n", out);

    ASSERT_EQ(0, exec_capture("echo 'abc' | tr 'a-z' 'A-Z'", out, sizeof(out)));
    ASSERT_STR_EQ("ABC\n", out);
#endif
    PASS();
}



SUITE(tr_suite) {
    // RUN_TEST(test_tr_basic);
    RUN_TEST(test_tr_tr_case_class);
    RUN_TEST(test_tr_tr);
}
DEFINE_TEST_MAIN(tr_suite)
