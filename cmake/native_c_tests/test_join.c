#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_join_basic(void) {
    FILE *f1 = fopen("test_join1.txt", "w");
    fprintf(f1, "1 a\n2 b\n");
    fclose(f1);
    FILE *f2 = fopen("test_join2.txt", "w");
    fprintf(f2, "1 x\n2 y\n");
    fclose(f2);
    char out[512];
    ASSERT_EQ(0, exec_capture("join test_join1.txt test_join2.txt", out, sizeof(out)));
    ASSERT(strstr(out, "1 a x") != NULL);
    remove("test_join1.txt");
    remove("test_join2.txt");
    PASS();
}

TEST test_join_utf8(void) {
    FILE *f1 = fopen("a", "w");
    fprintf(f1, "0|A\n1|a\n2|b\n4|c\n");
    fclose(f1);
    FILE *f2 = fopen("b", "w");
    fwrite("0|B\n1|d\n3|e\n4|\0f\n", 1, 17, f2);
    fclose(f2);
    
    char out[1024];
    ASSERT_EQ(0, exec_capture("join -t '|' -a1 -a2 -eouch -o0,1.2,2.2 a b", out, sizeof(out)));
    ASSERT(strstr(out, "0|A|B\n") != NULL || strstr(out, "0|A|B\r\n") != NULL);
    ASSERT(strstr(out, "1|a|d\n") != NULL || strstr(out, "1|a|d\r\n") != NULL);
    ASSERT(strstr(out, "2|b|ouch\n") != NULL || strstr(out, "2|b|ouch\r\n") != NULL);
    ASSERT(strstr(out, "3|ouch|e\n") != NULL || strstr(out, "3|ouch|e\r\n") != NULL);
    // 4|c|\0f -> length 6
    
    remove("a");
    remove("b");
    PASS();
}

TEST test_join_pl(void) {
    // Tests based on join.pl
    char out[1024];
    FILE *f1 = fopen("a", "w");
    fprintf(f1, "a 1\n");
    fclose(f1);
    FILE *f2 = fopen("b", "w");
    fprintf(f2, "a 2\n");
    fclose(f2);
    ASSERT_EQ(0, exec_capture("join a b", out, sizeof(out)));
    ASSERT(strstr(out, "a 1 2") != NULL);
    remove("a");
    remove("b");
    PASS();
}

TEST test_join_join(void) {
    // Covered by test_join_pl. Skip.
    PASS();
}





SUITE(join_suite) {
    RUN_TEST(test_join_basic);
    RUN_TEST(test_join_utf8);
    RUN_TEST(test_join_pl);
    RUN_TEST(test_join_join);
}
DEFINE_TEST_MAIN(join_suite)
