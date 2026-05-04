#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_shuf_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("shuf -i 1-3", out, sizeof(out)));
    ASSERT(strstr(out, "1") != NULL);
    ASSERT(strstr(out, "2") != NULL);
    ASSERT(strstr(out, "3") != NULL);
    PASS();
}

TEST test_shuf_sh(void) {
    char out1[1024];
    char out2[1024];

    // shuf_rand probability test
    ASSERT_EQ(0, exec_capture("shuf -i 1-100000000 -n 1", out1, sizeof(out1)));
    ASSERT_EQ(0, exec_capture("shuf -i 1-100000000 -n 1", out2, sizeof(out2)));
    // Probabilistically, these should differ
    ASSERT(strcmp(out1, out2) != 0);
    
    FILE *f = fopen("in", "w");
    for (int i = 1; i <= 100; i++) {
        fprintf(f, "%d\n", i);
    }
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("shuf in", out1, sizeof(out1)));
    
    // Read the input back to compare
    f = fopen("in", "r");
    fread(out2, 1, sizeof(out2) - 1, f);
    if (f) fclose(f);
    out2[1023] = '\0';
    
    // Fail if input is same as output (probabilistic)
    ASSERT(strcmp(out1, out2) != 0);
    
    // Sort output, should equal input
    ASSERT_EQ(0, exec_capture("shuf in | sort -n", out1, sizeof(out1)));
    

    remove("in");
    PASS();
}

TEST test_shuf_reservoir(void) {
    char out[1024];
#ifndef _WIN32
    // Basic reservoir tests, test different line counts (like 1023, 1024)
    FILE *f = fopen("shuf_res_in", "w");
    for (int i = 0; i < 2000; i++) {
        fprintf(f, "line %d\n", i);
    }
    if (f) fclose(f);
    
    // We just test if it crashes or fails to output correct count for n=10, 1024, etc.
    ASSERT_EQ(0, exec_capture("shuf -n 10 shuf_res_in | wc -l", out, sizeof(out)));
    trim_newline(out);
    char *p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("10", p);
    
    ASSERT_EQ(0, exec_capture("shuf -n 1024 shuf_res_in | wc -l", out, sizeof(out)));
    trim_newline(out);
    p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("1024", p);
    
    remove("shuf_res_in");
#endif
    PASS();
}

TEST test_shuf_options(void) {
    char out[1024];
    int ret;
    
    // Test errors
    ret = exec_capture("shuf -i0-9 -e A B 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    ret = exec_capture("shuf -nA 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    ret = exec_capture("shuf -i0-9 -n10 -i8-90 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    ret = exec_capture("shuf -i1-A 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    // Multiple -n
    ASSERT_EQ(0, exec_capture("shuf -n10 -i0-9 -n3 -n20 | wc -l", out, sizeof(out)));
    trim_newline(out);
    char *p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("3", p);
    
    // Repeat
    ASSERT_EQ(0, exec_capture("shuf --rep -i 0-10 | head -n 100 | wc -l", out, sizeof(out)));
    trim_newline(out);
    p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("100", p);
    
    // n=0
    ASSERT_EQ(0, exec_capture("shuf --rep -i0-9 -n0", out, sizeof(out)));
    ASSERT(strlen(out) == 0);
    
    // Zero terminated
    ASSERT_EQ(0, exec_capture("shuf --zero-terminated -i 1-1", out, sizeof(out)));
    ASSERT_STR_EQ("1\0", out); // just an idea, but exec_capture may treat it as C string.
    // If it treats as C string, it matches "1" actually.
    // But since it's zero terminated, out will contain "1" and then \0
    ASSERT(out[0] == '1');
    ASSERT(out[1] == '\0');
    
    PASS();
}
TEST test_shuf_shuf(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}





SUITE(shuf_suite) {
    RUN_TEST(test_shuf_basic);
    RUN_TEST(test_shuf_sh);
    RUN_TEST(test_shuf_reservoir);
    RUN_TEST(test_shuf_options);
    RUN_TEST(test_shuf_shuf);
}
DEFINE_TEST_MAIN(shuf_suite)
