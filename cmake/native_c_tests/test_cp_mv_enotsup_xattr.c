#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mount.h>



#if defined(__linux__)
#endif

static void cleanup() {
#if defined(__linux__)
    my_system("umount noxattr 2>/dev/null");
    my_system("umount xattr 2>/dev/null");
    my_system("umount xattr2 2>/dev/null");
    my_system("cmake -E rm -rf noxattr xattr xattr2 noxattr.bin xattr.bin xattr2.bin err exp out_a");
#endif
}

TEST test_cp_mv_enotsup_xattr(void) {
    cleanup();

#if !defined(__linux__)
    SKIPm("xattr tests requiring mount not supported on this platform");
#else
    if (geteuid() != 0) {
        SKIPm("must be run as root");
    }

    if (my_system("mkfs.ext2 -V >/dev/null 2>&1") != 0) {
        SKIPm("mkfs.ext2 not available");
    }

    if (my_system("setfattr --version >/dev/null 2>&1") != 0) {
        SKIPm("setfattr not available");
    }

    my_system("mkdir noxattr xattr xattr2");

    my_system("mount -t ramfs ramfs noxattr >/dev/null 2>&1");
    my_system("dd if=/dev/zero of=xattr.bin bs=8192 count=200 >/dev/null 2>&1");
    my_system("mkfs -t ext2 -F xattr.bin >/dev/null 2>&1");
    my_system("mount -oloop,user_xattr xattr.bin xattr >/dev/null 2>&1");

    my_system("dd if=/dev/zero of=xattr2.bin bs=8192 count=200 >/dev/null 2>&1");
    my_system("mkfs -t ext2 -F xattr2.bin >/dev/null 2>&1");
    my_system("mount -oloop,user_xattr xattr2.bin xattr2 >/dev/null 2>&1");

    my_system("echo test > xattr/a");
    if (my_system("setfattr -n user.foo -v bar xattr/a >/dev/null 2>&1") != 0) {
        cleanup();
        SKIPm("setfattr failed on xattr filesystem");
    }

    // cp -a xattr/a noxattr/
    ASSERT_EQ(0, exec_capture("cp -a xattr/a noxattr/", NULL, 0));
    struct stat st;
    ASSERT_EQ(0, stat("noxattr/a", &st));
    ASSERT(st.st_size > 0);
    unlink("noxattr/a");

    // cp --preserve=all xattr/a noxattr/
    ASSERT_EQ(0, exec_capture("cp --preserve=all xattr/a noxattr/", NULL, 0));
    ASSERT_EQ(0, stat("noxattr/a", &st));
    ASSERT(st.st_size > 0);

    // cp --preserve=all xattr/a noxattr/ (existing)
    ASSERT_EQ(0, exec_capture("cp --preserve=all xattr/a noxattr/", NULL, 0));
    unlink("noxattr/a");

    // cp -a --preserve=xattr xattr/a noxattr/ 2>err
    char out[1024];
    int res = exec_capture("cp -a --preserve=xattr xattr/a noxattr/", out, sizeof(out));
    ASSERT_NEQ(0, res);

    if (strstr(out, "Operation not supported") == NULL && strstr(out, "cp is built without xattr support") == NULL) {
        printf("Unexpected output: %s\n", out);
        ASSERT(0);
    }

    unlink("noxattr/a");

    // mv xattr/a noxattr/
    ASSERT_EQ(0, exec_capture("mv xattr/a noxattr/", NULL, 0));
    ASSERT_EQ(0, stat("noxattr/a", &st));
    ASSERT(st.st_size > 0);

    cleanup();
    PASS();
#endif
}





SUITE(cp_mv_enotsup_xattr_suite) {
    RUN_TEST(test_cp_mv_enotsup_xattr);
}
DEFINE_TEST_MAIN(cp_mv_enotsup_xattr_suite)
