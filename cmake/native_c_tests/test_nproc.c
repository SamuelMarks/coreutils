#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>





TEST test_nproc_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("nproc", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}

TEST test_nproc_avail(void) {
    char out1[512];
    char out2[512];
    ASSERT_EQ(0, exec_capture("nproc --all", out1, sizeof(out1)));
    ASSERT_EQ(0, exec_capture("env OMP_NUM_THREADS= nproc", out2, sizeof(out2)));
    int all = atoi(out1);
    int avail = atoi(out2);
    ASSERT(avail <= all);
    PASS();
}

TEST test_nproc_override(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("env OMP_NUM_THREADS=1 nproc", out, sizeof(out)));
    ASSERT_EQ(1, atoi(out));
    
    // OMP_THREAD_LIMIT takes precedence over OMP_NUM_THREADS
    ASSERT_EQ(0, exec_capture("env OMP_NUM_THREADS=2 OMP_THREAD_LIMIT=1 nproc", out, sizeof(out)));
    ASSERT_EQ(1, atoi(out));

    // --ignore overrides all
    ASSERT_EQ(0, exec_capture("env OMP_NUM_THREADS=2 nproc --ignore=1", out, sizeof(out)));
    int res = atoi(out);
    ASSERT(res == 1);
    
    PASS();
}

TEST test_nproc_positive(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("nproc", out, sizeof(out)));
    ASSERT(atoi(out) > 0);
    
    ASSERT_EQ(0, exec_capture("nproc --all", out, sizeof(out)));
    ASSERT(atoi(out) > 0);
    PASS();
}

TEST test_nproc_quota(void) {
    SKIPm("Skipped: nproc quota requires cgroups");
    PASS();
}
TEST test_nproc_quota_systemd(void) {
    SKIPm("Skipped: nproc quota systemd requires systemd");
    PASS();
}



SUITE(nproc_suite) {
    RUN_TEST(test_nproc_basic);
    RUN_TEST(test_nproc_avail);
    RUN_TEST(test_nproc_override);
    RUN_TEST(test_nproc_positive);
    RUN_TEST(test_nproc_quota);
    RUN_TEST(test_nproc_quota_systemd);
}
DEFINE_TEST_MAIN(nproc_suite)
