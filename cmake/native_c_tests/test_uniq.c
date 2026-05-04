#include "test_helper.h"
#include <locale.h>

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_uniq_basic(void) {
    FILE *f = fopen("test_uniq.txt", "w");
    fprintf(f, "a\na\nb\nb\n");
    if (f) fclose(f);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("uniq test_uniq.txt", out, sizeof(out)));
    
    ASSERT(strstr(out, "a\n") != NULL || strstr(out, "a\r\n") != NULL);
    ASSERT(strstr(out, "b\n") != NULL || strstr(out, "b\r\n") != NULL);
    
    remove("test_uniq.txt");
    PASS();
}

TEST test_uniq_pl(void) {
    char out[512];
    FILE *f = fopen("in", "w");
    fprintf(f, "a\na\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("uniq -c in", out, sizeof(out)));
    ASSERT(strstr(out, "2 a") != NULL);

    ASSERT_EQ(0, exec_capture("uniq -d in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("a", out);

    ASSERT_EQ(0, exec_capture("uniq -u in", out, sizeof(out)));
    trim_newline(out);
    ASSERT_EQ(0, out[0]); // unique lines: none

    f = fopen("in2", "w");
    fprintf(f, "a a\nb a\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq -f 1 in2", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("a a", out);

    remove("in");
    remove("in2");
    PASS();
}

TEST test_uniq_perf(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_perf", "w");
    for (int i = 0; i < 100; i++) {
        fprintf(f, "%d\n", i);
    }
    if (f) fclose(f);
    
    // uniq -f with huge skip used to perform horribly
    ASSERT_EQ(0, exec_capture("uniq -f 1000000000 in_perf", out, sizeof(out)));
    
    remove("in_perf");
#endif
    PASS();
}
TEST test_uniq_collate(void) {
    char out[1024];
#ifndef _WIN32
    if (setlocale(LC_ALL, "en_US.UTF-8") != NULL || setlocale(LC_ALL, "C.UTF-8") != NULL) {
        FILE *f = fopen("in_collate", "w");
        fprintf(f, "\xE2\x81\xBF\xE1\xB5\x98\xCB\xA1\xCB\xA1\n\xDC\xA2\xDC\x9D\xDC\xAA\xDC\x90\xDC\xA1\n");
        if (f) fclose(f);
        
        ASSERT_EQ(0, exec_capture("uniq in_collate | wc -l", out, sizeof(out)));
        trim_newline(out);
        char *p = out;
        while (*p == ' ' || *p == '\t') p++;
        ASSERT_STR_EQ("2", p);
        
        remove("in_collate");
    }
#endif
    PASS();
}
TEST test_uniq_uniq(void) {
    char out[1024];
#ifndef _WIN32
    // -z
    FILE *f = fopen("in_uniq", "w"); fprintf(f, "a\na\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq -z in_uniq", out, sizeof(out)));
    ASSERT(out[0] == 'a' && out[1] == '\n' && out[2] == 'a' && out[3] == '\n' && out[4] == '\0');

    // -s / -w
    f = fopen("in_uniq", "w"); fprintf(f, "a aaa\nb ab\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq -f 1 -s 1 in_uniq", out, sizeof(out)));
    ASSERT_STR_EQ("a aaa\nb ab\n", out);

    f = fopen("in_uniq", "w"); fprintf(f, "a aaa\nb aaa\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq -s 1 -f 1 in_uniq", out, sizeof(out)));
    ASSERT_STR_EQ("a aaa\n", out);

    // --group
    f = fopen("in_uniq", "w"); fprintf(f, "a\na\nb\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq --group=prepend in_uniq", out, sizeof(out)));
    ASSERT_STR_EQ("\na\na\n\nb\n", out);

    ASSERT_EQ(0, exec_capture("uniq --group=append in_uniq", out, sizeof(out)));
    ASSERT_STR_EQ("a\na\n\nb\n\n", out);

    // --all-repeated
    f = fopen("in_uniq", "w"); fprintf(f, "a\na\nb\nc\nc\n"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("uniq --all-repeated=separate in_uniq", out, sizeof(out)));
    ASSERT_STR_EQ("a\na\n\nc\nc\n", out);

    remove("in_uniq");
#endif
    PASS();
}



SUITE(uniq_suite) {
    RUN_TEST(test_uniq_basic);
    RUN_TEST(test_uniq_pl);
    RUN_TEST(test_uniq_perf);
    RUN_TEST(test_uniq_collate);
    RUN_TEST(test_uniq_uniq);
}
DEFINE_TEST_MAIN(uniq_suite)
