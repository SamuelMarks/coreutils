#include "test_helper.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>




TEST test_chcon_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("chcon --version", out, sizeof(out)));
    ASSERT(strstr(out, "chcon") != NULL);
    PASS();
}

TEST test_chcon_fail(void) {
    char out[512];
    ASSERT(exec_capture("chcon 2>/dev/null", out, sizeof(out)) != 0);
    ASSERT(exec_capture("chcon CON 2>/dev/null", out, sizeof(out)) != 0);
    
    FILE *f = fopen("f", "w");
    if (f) fclose(f);
    ASSERT(exec_capture("chcon --reference=f 2>/dev/null", out, sizeof(out)) != 0);
    remove("f");
    
    ASSERT(exec_capture("chcon -u anyone 2>/dev/null", out, sizeof(out)) != 0);
    PASS();
}

TEST test_chcon_chcon(void) {
    // Tests setting selinux contexts which requires root and active SELinux.
    // Skip in cross-platform native tests.
    PASS();
}






SUITE(chcon_suite) {
    RUN_TEST(test_chcon_basic);
    RUN_TEST(test_chcon_fail);
    RUN_TEST(test_chcon_chcon);
}
DEFINE_TEST_MAIN(chcon_suite)
