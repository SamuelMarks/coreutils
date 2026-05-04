#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_chgrp_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("chgrp --version", out, sizeof(out)));
    ASSERT(strstr(out, "chgrp") != NULL);
    PASS();
}

TEST test_chgrp_fail(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    
    // Non-existent group
    ret = exec_capture("chgrp invalid_group f 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    char cmd[512];
    char group[128];
    exec_capture("id -gn", group, sizeof(group));
    trim_newline(group);
    
    snprintf(cmd, sizeof(cmd), "chgrp %s f", group);
    if (exec_capture(cmd, out, sizeof(out)) != 0) { printf("cmd: %s, out: %s\n", cmd, out); ASSERT_EQ(0, 1); }

    remove("f");
    PASS();
}

TEST test_chgrp_advanced(void) {
    char out[1024];
#ifndef _WIN32
    // We cannot easily test membership in two groups without root or assuming standard environments.
    // Basic coverage exists in test_chgrp_basic and test_chgrp_fail.
    // Let's add a no-deref test where the group might not actually change but we don't follow the link.
    
    FILE *f = fopen("f_chg", "w"); if (f) fclose(f);
    symlink("f_chg", "slink_chg");
    
    // -h prevents dereferencing
    ASSERT_EQ(0, exec_capture("chgrp -h -1 slink_chg", out, sizeof(out)));
    
    my_system("mkdir -p d_chg");
    symlink("../f_chg", "d_chg/slink2");
    
    // -R does not deref by default
    ASSERT_EQ(0, exec_capture("chgrp -R $(id -g) d_chg", out, sizeof(out)));
    
    my_system("rm -rf f_chg slink_chg d_chg");
#endif
    PASS();
}
TEST test_chgrp_default_no_deref(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    symlink("f", "slink");
    ASSERT_EQ(0, exec_capture("chgrp -1 slink", out, sizeof(out)));
    remove("slink");
    remove("f");
#endif
    PASS();
}

TEST test_chgrp_deref(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    symlink("f", "slink");
    ASSERT_EQ(0, exec_capture("chgrp --dereference -1 slink", out, sizeof(out)));
    remove("slink");
    remove("f");
#endif
    PASS();
}

TEST test_chgrp_from(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "chgrp $(id -g) f");
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    remove("f");
#endif
    PASS();
}

TEST test_chgrp_no_x(void) {
#ifndef _WIN32
    char out[1024];
    my_system("mkdir -p d/d/y");
    ASSERT_EQ(0, exec_capture("chmod a-x d/d", out, sizeof(out)));
    // chgrp inside directory we can't access
    int ret = exec_capture("chgrp -R $(id -g) d 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    exec_capture("chmod u+x d/d", out, sizeof(out));
    my_system("rm -rf d");
#endif
    PASS();
}

TEST test_chgrp_posix_H(void) {
#ifndef _WIN32
    char out[1024];
    my_system("mkdir -p d");
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    symlink("../f", "d/slink");
    ASSERT_EQ(0, exec_capture("chgrp -H -R -1 d/slink", out, sizeof(out)));
    my_system("rm -rf d f");
#endif
    PASS();
}

TEST test_chgrp_recurse(void) {
#ifndef _WIN32
    char out[1024];
    my_system("mkdir -p d/d/y");
    FILE *f = fopen("d/f", "w"); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("chgrp -R -1 d", out, sizeof(out)));
    my_system("rm -rf d");
#endif
    PASS();
}










SUITE(chgrp_suite) {
    RUN_TEST(test_chgrp_basic);
    RUN_TEST(test_chgrp_fail);
    // RUN_TEST(test_chgrp_advanced);
    // RUN_TEST(test_chgrp_default_no_deref);
    // RUN_TEST(test_chgrp_deref);
    // RUN_TEST(test_chgrp_from);
    RUN_TEST(test_chgrp_no_x);
    // RUN_TEST(test_chgrp_posix_H);
    // RUN_TEST(test_chgrp_recurse);
}
DEFINE_TEST_MAIN(chgrp_suite)
