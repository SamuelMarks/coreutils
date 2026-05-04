#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_comm_basic(void) {
    FILE *f1 = fopen("test_comm1.txt", "w");
    fprintf(f1, "a\nb\nc\n");
    fclose(f1);
    FILE *f2 = fopen("test_comm2.txt", "w");
    fprintf(f2, "b\nc\nd\n");
    fclose(f2);
    char out[512];
    ASSERT_EQ(0, exec_capture("comm test_comm1.txt test_comm2.txt", out, sizeof(out)));
    ASSERT(strstr(out, "a") != NULL);
    ASSERT(strstr(out, "b") != NULL);
    remove("test_comm1.txt");
    remove("test_comm2.txt");
    PASS();
}

TEST test_comm_pl(void) {
    char out[512];
    
    FILE *f1 = fopen("a", "w");
    fprintf(f1, "1\n3\n3\n3\n");
    fclose(f1);
    
    FILE *f2 = fopen("b", "w");
    fprintf(f2, "2\n2\n3\n3\n3\n");
    fclose(f2);
    
    ASSERT_EQ(0, exec_capture("comm a b", out, sizeof(out)));
    ASSERT(strstr(out, "1\n\t2\n\t2\n\t\t3\n\t\t3\n\t\t3\n") != NULL || strstr(out, "1\r\n\t2\r\n\t2\r\n\t\t3\r\n\t\t3\r\n\t\t3\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("comm -1 a b", out, sizeof(out)));
    ASSERT(strstr(out, "2\n2\n\t3\n\t3\n\t3\n") != NULL || strstr(out, "2\r\n2\r\n\t3\r\n\t3\r\n\t3\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("comm -2 a b", out, sizeof(out)));
    ASSERT(strstr(out, "1\n\t3\n\t3\n\t3\n") != NULL || strstr(out, "1\r\n\t3\r\n\t3\r\n\t3\r\n") != NULL);

    ASSERT_EQ(0, exec_capture("comm -3 a b", out, sizeof(out)));
    ASSERT(strstr(out, "1\n\t2\n\t2\n") != NULL || strstr(out, "1\r\n\t2\r\n\t2\r\n") != NULL);
    
    remove("a");
    remove("b");

    PASS();
}




SUITE(comm_suite) {
    RUN_TEST(test_comm_basic);
    RUN_TEST(test_comm_pl);
}
DEFINE_TEST_MAIN(comm_suite)
