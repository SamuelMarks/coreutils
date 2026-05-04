#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_groups_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("groups", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}

TEST test_groups_dash(void) {
    char out[512];
    ASSERT(exec_capture("groups :invalid -- 2>&1", out, sizeof(out)) != 0);
    ASSERT(strstr(out, "groups: ':invalid': no such user") != NULL || strstr(out, "groups: unknown user :invalid") != NULL || strstr(out, "no such user") != NULL);
    PASS();
}

TEST test_groups_process_all(void) {
    char out[1024];
    ASSERT(exec_capture("groups :1 :2 :3 2>&1", out, sizeof(out)) != 0);
    // Count newlines to check if it processed all 3.
    int lines = 0;
    for (int i = 0; out[i]; i++) {
        if (out[i] == '\n') lines++;
    }
    ASSERT(lines >= 3);
    PASS();
}

TEST test_groups_version(void) {
    char out1[1024];
    ASSERT_EQ(0, exec_capture("groups --version", out1, sizeof(out1)));
    char out2[1024];
    ASSERT_EQ(0, exec_capture("id --version", out2, sizeof(out2)));
    
    // We just check they both output something
    ASSERT(strlen(out1) > 10);
    ASSERT(strlen(out2) > 10);
    PASS();
}





SUITE(groups_suite) {
    RUN_TEST(test_groups_basic);
    RUN_TEST(test_groups_dash);
    RUN_TEST(test_groups_process_all);
    RUN_TEST(test_groups_version);
}
DEFINE_TEST_MAIN(groups_suite)
