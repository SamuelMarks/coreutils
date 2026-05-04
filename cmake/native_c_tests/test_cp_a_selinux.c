#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
#ifndef _WIN32
    my_system("umount mnt 2>/dev/null");
    my_system("cmake -E rm -rf c d e f backup restore parents parents_dest c_d Z1 Z1_d Z2 Z3 Z3_d existing blob mnt");
#endif
}

TEST test_cp_a_selinux(void) {
    cleanup();

#if defined(_WIN32) || defined(__APPLE__)
    SKIPm("SELinux not supported on this platform");
#else
    if (geteuid() != 0) {
        SKIPm("must be run as root");
    }

    if (my_system("selinuxenabled 2>/dev/null") != 0) {
        SKIPm("SELinux is disabled or not available");
    }

    const char *ctx = "root:object_r:tmp_t:s0";

    my_system("touch c");
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "chcon %s c 2>/dev/null", ctx);
    if (my_system(cmd) != 0) {
        ctx = "root:object_r:tmp_t";
        snprintf(cmd, sizeof(cmd), "chcon %s c 2>/dev/null", ctx);
        if (my_system(cmd) != 0) {
            cleanup();
            SKIPm("Failed to set context");
        }
    }

    ASSERT_EQ(0, exec_capture("cp -a c d", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp --preserve=context c e", NULL, 0));
    ASSERT_EQ(0, exec_capture("cp --preserve=all c f", NULL, 0));

    // verify
    char out[1024];
    my_system("ls -Z d > out_d");
    FILE *f = fopen("out_d", "r");
    if (f) {
        fread(out, 1, sizeof(out)-1, f);
        if (f) fclose(f);
        ASSERT(strstr(out, ctx) != NULL);
    }
    unlink("out_d");

    cleanup();
    PASS();
#endif
}





SUITE(cp_a_selinux_suite) {
    RUN_TEST(test_cp_a_selinux);
}
DEFINE_TEST_MAIN(cp_a_selinux_suite)
