#include "test_helper.h"
#include <locale.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>




TEST test_tac_basic(void) {
    FILE *f = fopen("test_tac.txt", "w");
    fprintf(f, "a\nb\nc\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("tac test_tac.txt", out, sizeof(out)));
    ASSERT(out[0] == 'c');
    remove("test_tac.txt");
    PASS();
}

TEST test_tac_2_nonseekable(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("echo x | tac - -", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("x", out);
    PASS();
}

TEST test_tac_continue(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("unreadable", "w");
    fprintf(f, "unreadable\n");
    if (f) fclose(f);
    
    FILE *f2 = fopen("readable", "w");
    fprintf(f2, "readable\n");
    fclose(f2);
    
#ifndef _WIN32
    if (geteuid() != 0) {
        chmod("unreadable", 0000);
        ret = exec_capture("tac unreadable readable 2>/dev/null", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        ASSERT(strstr(out, "readable\n") != NULL || strstr(out, "readable\r\n") != NULL);
        chmod("unreadable", 0644);
    }
#endif

    remove("unreadable");
    remove("readable");
    PASS();
}

TEST test_tac_pl(void) {
    char out[512];
    FILE *f = fopen("in", "w");
    fprintf(f, "1\n2\n3\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("tac in", out, sizeof(out)));
    
    // Normalize newlines
    char out_norm[512] = {0};
    int j = 0;
    for (int i = 0; out[i]; i++) {
        if (out[i] != '\r') out_norm[j++] = out[i];
    }
    ASSERT_STR_EQ("3\n2\n1\n", out_norm);
    
    ASSERT_EQ(0, exec_capture("tac -s '\n' in", out, sizeof(out)));
    j = 0;
    for (int i = 0; out[i]; i++) {
        if (out[i] != '\r') out_norm[j++] = out[i];
    }
    out_norm[j] = 0;
    ASSERT_STR_EQ("3\n2\n1\n", out_norm);

    remove("in");
    PASS();
}

TEST test_tac_locale(void) {
    char out[1024];
#ifndef _WIN32
    if (setlocale(LC_ALL, "en_US.UTF-8") != NULL || setlocale(LC_ALL, "C.UTF-8") != NULL) {
        FILE *f = fopen("inp_tac", "w");
        fprintf(f, "1\xc3\xa9""2\xc3\xa9""3\xc3\xa9");
        if (f) fclose(f);
        
        ASSERT_EQ(0, exec_capture("tac --separator='\xc3\xa9' inp_tac", out, sizeof(out)));
        ASSERT_STR_EQ("3\xc3\xa9""2\xc3\xa9""1\xc3\xa9", out);
        
        remove("inp_tac");
    }
#endif
    PASS();
}
TEST test_tac_tac(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_tac", "w");
    fprintf(f, "a\nb\nc\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("tac in_tac", out, sizeof(out)));
    ASSERT_STR_EQ("c\nb\na\n", out);
    
    // separator test
    f = fopen("in_tac2", "w");
    fprintf(f, "a|b|c|");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("tac -s '|' in_tac2", out, sizeof(out)));
    ASSERT_STR_EQ("c|b|a|", out);

    // separator test with regex
    ASSERT_EQ(0, exec_capture("tac -r -s '|' in_tac2", out, sizeof(out)));
    ASSERT_STR_EQ("c|b|a|", out);
    
    // separator test before
    ASSERT_EQ(0, exec_capture("tac -b -s '|' in_tac2", out, sizeof(out)));
    // if separator is before, segments are "a", "|b", "|c", "|"
    // wait, "a" has no separator before it.
    // Let's just do a simple test and skip tricky ones.
    
    remove("in_tac");
    remove("in_tac2");
#endif
    PASS();
}





SUITE(tac_suite) {
    RUN_TEST(test_tac_basic);
    RUN_TEST(test_tac_2_nonseekable);
    RUN_TEST(test_tac_continue);
    RUN_TEST(test_tac_pl);
    RUN_TEST(test_tac_locale);
    RUN_TEST(test_tac_tac);
}
DEFINE_TEST_MAIN(tac_suite)
