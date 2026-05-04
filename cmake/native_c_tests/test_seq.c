#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>





TEST test_seq_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("seq 3", out, sizeof(out)));
    /* Ensure the sequence 1, 2, 3 is present in the output */
    ASSERT(strstr(out, "1") != NULL);
    ASSERT(strstr(out, "2") != NULL);
    ASSERT(strstr(out, "3") != NULL);
    PASS();
}

TEST test_seq_pl(void) {
    char out[1024];
    int ret;
    
    ASSERT_EQ(0, exec_capture("seq 3", out, sizeof(out)));
    ASSERT(strstr(out, "1\n2\n3\n") != NULL || strstr(out, "1\r\n2\r\n3\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("seq -w 01 02", out, sizeof(out)));
    ASSERT(strstr(out, "01\n02\n") != NULL || strstr(out, "01\r\n02\r\n") != NULL);
    
    // Fractional step
    ASSERT_EQ(0, exec_capture("seq 1 0.5 2", out, sizeof(out)));
    ASSERT(strstr(out, "1.0\n1.5\n2.0\n") != NULL || strstr(out, "1.0\r\n1.5\r\n2.0\r\n") != NULL);
    
    // Floating point format
    ASSERT_EQ(0, exec_capture("seq -f %2.1f 1.5 0.5 2", out, sizeof(out)));
    ASSERT(strstr(out, "1.5\n2.0\n") != NULL || strstr(out, "1.5\r\n2.0\r\n") != NULL);
    
    // Error cases
    ret = exec_capture("seq 1 0 2 2>&1", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT(strstr(out, "invalid Zero increment value") != NULL);

    ret = exec_capture("seq 1 nan 2 2>&1", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT(strstr(out, "not-a-number") != NULL);

    PASS();
}

TEST test_seq_extra_number(void) {
    char out[1024];
    
    // Test 1
    ASSERT_EQ(0, exec_capture("seq 0 0.000001 0.000003", out, sizeof(out)));
    ASSERT(strstr(out, "0.000001") != NULL);
    ASSERT(strstr(out, "0.000002") != NULL);
    ASSERT(strstr(out, "0.000003") != NULL);
    
    // Test 2
    ASSERT_EQ(0, exec_capture("seq -f \"%g=\" 1000000 1000000", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("1e+06=", out);
    
    PASS();
}

TEST test_seq_io_errors(void) {
    char out[1024];
#ifndef _WIN32
    struct stat st;
    if (stat("/dev/full", &st) == 0) {
        { int _r = exec_capture("seq 1 inf > /dev/full 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("seq 1.1 .1 inf > /dev/full 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        { int _r = exec_capture("seq 1 0.0001 99999999 > /dev/full 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    }
#endif
    PASS();
}

TEST test_seq_precision(void) {
    char out[1024];
    
    ASSERT_EQ(0, exec_capture("seq 999999 inf | head -n2", out, sizeof(out)));
    ASSERT(strstr(out, "999999\n1000000\n") != NULL || strstr(out, "999999\r\n1000000\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq 0xF423F 0xF4240", out, sizeof(out)));
    ASSERT(strstr(out, "999999\n1000000\n") != NULL || strstr(out, "999999\r\n1000000\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq 1 .1 inf | head -n2", out, sizeof(out)));
    ASSERT(strstr(out, "1.0\n1.1\n") != NULL || strstr(out, "1.0\r\n1.1\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq 1 0x1p-1 2", out, sizeof(out)));
    ASSERT(strstr(out, "1\n1.5\n2\n") != NULL || strstr(out, "1\r\n1.5\r\n2\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq 1 .1 0x2 | head -n2", out, sizeof(out)));
    ASSERT(strstr(out, "1.0\n1.1\n") != NULL || strstr(out, "1.0\r\n1.1\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq 1.1e1 12", out, sizeof(out)));
    ASSERT(strstr(out, "11\n12\n") != NULL || strstr(out, "11\r\n12\r\n") != NULL);
    
    ASSERT_EQ(0, exec_capture("seq -w 1.1e4 | head -n1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("00001", out);
    
    PASS();
}

TEST test_seq_long_double(void) {
    char out[1024];
    // Simple test for inf support on large
    ASSERT_EQ(0, exec_capture("seq 18446744073709551617 inf | head -n1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("18446744073709551617", out);
    PASS();
}
TEST test_seq_seq_epipe(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}

TEST test_seq_seq_locale(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}

TEST test_seq_seq(void) {
    char out[1024];
    
    ASSERT_EQ(0, exec_capture("seq 10", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n", out);
    
    ASSERT_EQ(0, exec_capture("seq -10 10 10", out, sizeof(out)));
    ASSERT_STR_EQ("-10\n0\n10\n", out);
    
    ASSERT_EQ(0, exec_capture("seq 1 -1 0", out, sizeof(out)));
    ASSERT_STR_EQ("1\n0\n", out);
    
    ASSERT_EQ(0, exec_capture("seq 0.8 0.1 0.9", out, sizeof(out)));
    ASSERT_STR_EQ("0.8\n0.9\n", out);
    
    ASSERT_EQ(0, exec_capture("seq 0.1 0.99 1.99", out, sizeof(out)));
    ASSERT_STR_EQ("0.10\n1.09\n", out);
    
    ASSERT_EQ(0, exec_capture("seq -w 1 -1 -1", out, sizeof(out)));
    ASSERT_STR_EQ("01\n00\n-1\n", out);
    
    ASSERT_EQ(0, exec_capture("seq -f %2.1f 1.5 .5 2", out, sizeof(out)));
    ASSERT_STR_EQ("1.5\n2.0\n", out);
    
    ASSERT_EQ(0, exec_capture("seq -s '' 1 5", out, sizeof(out)));
    ASSERT_STR_EQ("12345\n", out);
    
    PASS();
}







SUITE(seq_suite) {
    RUN_TEST(test_seq_basic);
    RUN_TEST(test_seq_pl);
    RUN_TEST(test_seq_extra_number);
    RUN_TEST(test_seq_io_errors);
    RUN_TEST(test_seq_precision);
    RUN_TEST(test_seq_long_double);
    // RUN_TEST(test_seq_seq_epipe);
    // RUN_TEST(test_seq_seq_locale);
    RUN_TEST(test_seq_seq);
}
DEFINE_TEST_MAIN(seq_suite)
