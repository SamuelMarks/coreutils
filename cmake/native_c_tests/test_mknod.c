#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>




TEST test_mknod_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("mknod --version", out, sizeof(out)));
    ASSERT(strstr(out, "mknod") != NULL);
    PASS();
}

TEST test_mknod_umask(void) {
#ifndef _WIN32
    // Ensure fifos are supported
    if (mkfifo("fifo", 0600) != 0) {
        PASS(); // Skip if mkfifo is not supported
    }
    remove("fifo");

    mode_t old_umask = umask(0777);
    char out[512];

    ASSERT_EQ(0, exec_capture("mknod -m 734 f1 p", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("f1", &st));
    ASSERT(S_ISFIFO(st.st_mode));
    ASSERT_EQ(0734, st.st_mode & 07777);

    ASSERT_EQ(0, exec_capture("mkfifo -m 734 f2", out, sizeof(out)));
    ASSERT_EQ(0, stat("f2", &st));
    ASSERT(S_ISFIFO(st.st_mode));
    ASSERT_EQ(0734, st.st_mode & 07777);

    ASSERT_EQ(0, exec_capture("mkdir -m 734 f3", out, sizeof(out)));
    ASSERT_EQ(0, stat("f3", &st));
    ASSERT(S_ISDIR(st.st_mode));
    // Test says drwx-wxr-- or drwx-wsr-- so could be 0734 or 02734
    ASSERT((st.st_mode & 07777) == 0734 || (st.st_mode & 07777) == 02734);

    ASSERT_EQ(0, exec_capture("mknod --mode='ug+rw,o+r' f4 p", out, sizeof(out)));
    ASSERT_EQ(0, stat("f4", &st));
    ASSERT(S_ISFIFO(st.st_mode));
    // ls -dgo f4 | cut -b-10 == prw-rw-rw- -> 0666 ? 
    // umask 777. ug+rw,o+r applied to default (which is usually 0666 for fifos maybe modified by umask).
    // The test expects prw-rw-rw- which is 0666.
    ASSERT_EQ(0666, st.st_mode & 0777);

    ASSERT_EQ(0, exec_capture("mkfifo --mode='ug+rw,o+r' f5", out, sizeof(out)));
    ASSERT_EQ(0, stat("f5", &st));
    ASSERT(S_ISFIFO(st.st_mode));
    ASSERT_EQ(0666, st.st_mode & 0777);

    struct stat st_cur;
    stat(".", &st_cur);
    if (!(st_cur.st_mode & S_ISGID)) {
        ASSERT_EQ(0, exec_capture("mkdir --mode='ug+rw,o+r' f6", out, sizeof(out)));
        ASSERT_EQ(0, stat("f6", &st));
        ASSERT(S_ISDIR(st.st_mode));
        // test "$mode" = drwxrwxrwx -> 0777
        ASSERT_EQ(0777, st.st_mode & 07777);
        rmdir("f6");
    }

    umask(old_umask);
    remove("f1");
    remove("f2");
    rmdir("f3");
    remove("f4");
    remove("f5");
#endif
    PASS();
}

TEST test_mknod_sh(void) {
    char out[1024];
#ifndef _WIN32
    umask(0000);
    
    // mknod -m 734
    ASSERT_EQ(0, exec_capture("mknod -m 734 f1_mk p", out, sizeof(out)));
    struct stat st;
    stat("f1_mk", &st);
    ASSERT_EQ(0734, st.st_mode & 0777);
    ASSERT((st.st_mode & S_IFMT) == S_IFIFO);
    
    // mkdir -m 734
    ASSERT_EQ(0, exec_capture("mkdir -m 734 f3_mk", out, sizeof(out)));
    stat("f3_mk", &st);
    ASSERT_EQ(0734, st.st_mode & 0777);
    
    // mode=ug+rw,o+r
    ASSERT_EQ(0, exec_capture("mknod --mode='ug+rw,o+r' f4_mk p", out, sizeof(out)));
    stat("f4_mk", &st);
    ASSERT_EQ(0666, st.st_mode & 0777);
    
    my_system("rm -rf f1_mk f3_mk f4_mk");
#endif
    PASS();
}



SUITE(mknod_suite) {
    RUN_TEST(test_mknod_basic);
    RUN_TEST(test_mknod_umask);
    RUN_TEST(test_mknod_sh);
}
DEFINE_TEST_MAIN(mknod_suite)
