#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>





TEST test_tsort_basic(void) {
    FILE *f = fopen("test_tsort.txt", "w");
    fprintf(f, "a b\nb c\n");
    if (f) fclose(f);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("tsort test_tsort.txt", out, sizeof(out)));
    
    ASSERT(strstr(out, "a") != NULL);
    ASSERT(strstr(out, "b") != NULL);
    ASSERT(strstr(out, "c") != NULL);
    
    remove("test_tsort.txt");
    PASS();
}

TEST test_tsort_pl(void) {
    char out[512];
    int ret;
    FILE *f = fopen("f", "w");
    fprintf(f, "t b\nt s\ns t\n");
    if (f) fclose(f);
    
    ret = exec_capture("tsort f 2>&1", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT(strstr(out, "tsort: f: input contains a loop:\ntsort: s\ntsort: t\n") != NULL || strstr(out, "tsort: f: input contains a loop:\r\ntsort: s\r\ntsort: t\r\n") != NULL);
    
    f = fopen("in", "w");
    fprintf(f, "a b c c d e\ng g\nf g e f\nh h\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("tsort in", out, sizeof(out)));
    ASSERT(strstr(out, "a\nc\nd\nh\nb\ne\nf\ng\n") != NULL || strstr(out, "a\r\nc\r\nd\r\nh\r\nb\r\ne\r\nf\r\ng\r\n") != NULL);

    remove("f");
    remove("in");
    PASS();
}




SUITE(tsort_suite) {
    RUN_TEST(test_tsort_basic);
    RUN_TEST(test_tsort_pl);
}
DEFINE_TEST_MAIN(tsort_suite)
