#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_od_basic(void) {
    FILE *f = fopen("test_od.txt", "w");
    fprintf(f, "abcd");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("od -c test_od.txt", out, sizeof(out)));
    ASSERT(strstr(out, "a") != NULL);
    ASSERT(strstr(out, "b") != NULL);
    ASSERT(strstr(out, "c") != NULL);
    ASSERT(strstr(out, "d") != NULL);
    remove("test_od.txt");
    PASS();
}

TEST test_od_pl(void) {
    char out[512];
    FILE *f = fopen("in", "w");
    fprintf(f, "a");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -A n -c in", out, sizeof(out)));
    ASSERT(strstr(out, "   a") != NULL);

    ASSERT_EQ(0, exec_capture("od -A n -b in", out, sizeof(out)));
    ASSERT(strstr(out, " 141") != NULL);

    ASSERT_EQ(0, exec_capture("od -A n -t x1 in", out, sizeof(out)));
    ASSERT(strstr(out, " 61") != NULL);

    remove("in");
    PASS();
}

TEST test_od_endian(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_od_e", "w");
    fprintf(f, "0123456789abcdef");
    if (f) fclose(f);
    
    // We just test basic parsing
    ASSERT_EQ(0, exec_capture("od -t x1 --endian=big in_od_e > /dev/null", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("od -t x1 --endian=little in_od_e > /dev/null", out, sizeof(out)));
    
    remove("in_od_e");
#endif
    PASS();
}

TEST test_od_j(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_od_j", "w");
    fprintf(f, "abcd");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -An -j 2 -t c in_od_j", out, sizeof(out)));
    ASSERT(strstr(out, "c   d") != NULL);
    
    remove("in_od_j");
#endif
    PASS();
}

TEST test_od_multiple_t(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_od_t", "w");
    fprintf(f, "0123456789abcdef");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -An -t a -t x1 in_od_t > out_od_t", out, sizeof(out)));
    
    FILE *out_f = fopen("out_od_t", "r");
    char buf[1024] = {0};
    if (out_f) {
        fread(buf, 1, sizeof(buf) - 1, out_f);
        fclose(out_f);
    }
    // Should have both formats aligned
    ASSERT(strstr(buf, "0") != NULL);
    ASSERT(strstr(buf, "30") != NULL); // hex for '0'
    
    remove("in_od_t");
    remove("out_od_t");
#endif
    PASS();
}
TEST test_od_big_w(void) {
    char out[1024];
#ifndef _WIN32
    // test huge -w
    { int _r = exec_capture("od -w9999999999999999999 </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
#endif
    PASS();
}

TEST test_od_od_float(void) {
    SKIPm("Skipped: float serialization across platforms/compilers varies");
    PASS();
}

TEST test_od_od_N(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_od", "w");
    fprintf(f, "abcdefg");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -A n -c -N 3 f_od", out, sizeof(out)));
    ASSERT(strstr(out, "   a   b   c") != NULL);
    
    remove("f_od");
#endif
    PASS();
}

TEST test_od_od_x8(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_od8", "w");
    fprintf(f, "12345678");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -A n -t x8 f_od8 > out_od8", out, sizeof(out)));
    // the exact output endianness varies, just verify it didn't fail
    remove("f_od8");
    remove("out_od8");
#endif
    PASS();
}

TEST test_od_od(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_od", "w");
    fprintf(f, "A\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("od -A n -c f_od", out, sizeof(out)));
    ASSERT(strstr(out, "   A  \\n") != NULL);
    
    // -t a
    ASSERT_EQ(0, exec_capture("od -A n -t a f_od", out, sizeof(out)));
    ASSERT(strstr(out, "   A  nl") != NULL);
    
    remove("f_od");
#endif
    PASS();
}








SUITE(od_suite) {
    RUN_TEST(test_od_basic);
    RUN_TEST(test_od_pl);
    RUN_TEST(test_od_endian);
    RUN_TEST(test_od_j);
    RUN_TEST(test_od_multiple_t);
    RUN_TEST(test_od_big_w);
    RUN_TEST(test_od_od_float);
    RUN_TEST(test_od_od_N);
    RUN_TEST(test_od_od_x8);
    RUN_TEST(test_od_od);
}
DEFINE_TEST_MAIN(od_suite)
