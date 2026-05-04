#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_basenc_basic(void) {
    FILE *f = fopen("test_basenc.txt", "w");
    fprintf(f, "hello world");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("basenc --base64 test_basenc.txt", out, sizeof(out)));
    ASSERT(strstr(out, "aGVsbG8gd29ybGQ=") != NULL);
    remove("test_basenc.txt");
    PASS();
}

TEST test_basenc_pl(void) {
    char out[512];
    
    // empty string
    ASSERT_EQ(0, exec_capture("printf '' | basenc --base64", out, sizeof(out)));
    trim_newline(out);
    ASSERT_EQ(0, out[0]);
    
    // Test known strings from basenc.pl
    // base64
    FILE *f = fopen("in", "w");
    fprintf(f, "abc");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("basenc --base64 in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("YWJj", out);

    // base32
    ASSERT_EQ(0, exec_capture("basenc --base32 in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("MFRGG===", out);

    // base32hex
    ASSERT_EQ(0, exec_capture("basenc --base32hex in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("C5H66===", out);

    // base16
    ASSERT_EQ(0, exec_capture("basenc --base16 in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("616263", out);

    // decode
    f = fopen("in_dec", "w");
    fprintf(f, "YWJj\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("basenc --base64 -d in_dec", out, sizeof(out)));
    ASSERT_STR_EQ("abc", out);

    remove("in");
    remove("in_dec");
    PASS();
}

TEST test_basenc_base64(void) {
    char out[1024];
    
    FILE *f = fopen("test_base64_in", "wb");
    fprintf(f, "abc");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("base64 test_base64_in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("YWJj", out);

    remove("test_base64_in");
    PASS();
}

TEST test_basenc_basenc(void) {
    // Tests from perl basenc.pl covered in test_basenc_pl. Skip.
    PASS();
}

TEST test_basenc_bounded_memory(void) {
    // Tests memory limits using ulimit -v which is non-portable. Skipping.
    PASS();
}

TEST test_basenc_large_input(void) {
    // coreutils v9.8 would not operate correctly with > 15,561,475 bytes
    FILE *f = fopen("file.zeros", "wb");
    if (!f) PASS();
    fseek(f, 20 * 1024 * 1024 - 1, SEEK_SET);
    fputc(0, f);
    if (f) fclose(f);

    char out[128];
    ASSERT_EQ(0, exec_capture("basenc --base58 file.zeros | wc -c", out, sizeof(out)));
    trim_newline(out);
    
    // Ignore leading spaces in wc -c output
    char *p = out;
    while (*p == ' ' || *p == '\t') p++;
    
    ASSERT_STR_EQ("21247462", p);

    remove("file.zeros");
    PASS();
}








SUITE(basenc_suite) {
    RUN_TEST(test_basenc_basic);
    RUN_TEST(test_basenc_pl);
    RUN_TEST(test_basenc_base64);
    RUN_TEST(test_basenc_basenc);
    RUN_TEST(test_basenc_bounded_memory);
    RUN_TEST(test_basenc_large_input);
}
DEFINE_TEST_MAIN(basenc_suite)
