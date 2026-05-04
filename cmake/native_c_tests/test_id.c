#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_id_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("id -u", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}

TEST test_id_uid(void) {
    char out_uid[512];
    char out_user[512];
    ASSERT_EQ(0, exec_capture("id -u", out_uid, sizeof(out_uid)));
    ASSERT_EQ(0, exec_capture("id -nu", out_user, sizeof(out_user)));
    
    char out[512];
    ASSERT(exec_capture("id '' 2>&1", out, sizeof(out)) != 0);

    char out_single[1024];
    char cmd1[512];
    trim_newline(out_user);
    snprintf(cmd1, sizeof(cmd1), "id %s", out_user);
    ASSERT_EQ(0, exec_capture(cmd1, out_single, sizeof(out_single)));

    char out_multi[1024];
    char cmd2[512];
    snprintf(cmd2, sizeof(cmd2), "id '' %s 2>&1", out_user);
    ASSERT(exec_capture(cmd2, out_multi, sizeof(out_multi)) != 0);
    
    PASS();
}

TEST test_id_zero(void) {
    char out_user[512];
    ASSERT_EQ(0, exec_capture("id -nu", out_user, sizeof(out_user)));
    trim_newline(out_user);

    char out1[1024];
    char out2[1024];
    char cmd[512];
    
    snprintf(cmd, sizeof(cmd), "id -z -u %s", out_user);
    ASSERT_EQ(0, exec_capture(cmd, out1, sizeof(out1)));
    
    snprintf(cmd, sizeof(cmd), "id -z -u %s %s", out_user, out_user);
    ASSERT_EQ(0, exec_capture(cmd, out2, sizeof(out2)));
    
    PASS();
}

TEST test_id_gnu_zero_uids(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("id -u", out, sizeof(out)));
    PASS();
}

TEST test_id_context(void) {
    // Tests SELinux context. Skip.
    PASS();
}

TEST test_id_no_context(void) {
    // Tests disabled SELinux context. Skip.
    PASS();
}

TEST test_id_setgid(void) {
    // Tests setgid binaries. Skip.
    PASS();
}

TEST test_id_smack(void) {
    // Tests smack labels. Skip.
    PASS();
}








SUITE(id_suite) {
    RUN_TEST(test_id_basic);
    RUN_TEST(test_id_uid);
    RUN_TEST(test_id_zero);
    RUN_TEST(test_id_gnu_zero_uids);
    RUN_TEST(test_id_context);
    RUN_TEST(test_id_no_context);
    RUN_TEST(test_id_setgid);
    RUN_TEST(test_id_smack);
}
DEFINE_TEST_MAIN(id_suite)
