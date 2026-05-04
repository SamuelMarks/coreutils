#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mount.h>



#if defined(__linux__)
#endif

static void cleanup() {
#if defined(__linux__)
    my_system("umount mnt 2>/dev/null");
#endif
    my_system("cmake -E rm -rf fs.img mnt path2");
}

TEST test_cp_cross_dev_symlink(void) {
    cleanup();

#if !defined(__linux__)
    SKIPm("loopback mount testing not supported on this platform");
#else
    if (geteuid() != 0) {
        SKIPm("must be run as root");
    }

    if (my_system("mkfs -t ext4 -V >/dev/null 2>&1") != 0) {
        SKIPm("mkfs.ext4 not available");
    }

    my_system("truncate -s100M fs.img");
    if (my_system("mkfs -t ext4 -F fs.img >/dev/null 2>&1") != 0) {
        cleanup();
        SKIPm("failed to create ext4 file system");
    }

    my_system("mkdir mnt");
    if (my_system("mount -oloop fs.img mnt >/dev/null 2>&1") != 0) {
        cleanup();
        SKIPm("failed to mount ext4 file system");
    }

    my_system("mkdir mnt/path1");
    my_system("touch mnt/path1/file");
    my_system("mkdir path2");
    
    // Original does: cd path2 && ln -s ../mnt/path1/file
    if (symlink("../mnt/path1/file", "path2/file") != 0) {
        cleanup();
        SKIPm("symlink failed");
    }

    // Original does: cp -dsf ../mnt/path1/file . (inside path2)
    // We execute it locally from root test dir pointing to path2
    ASSERT_EQ(0, exec_capture("cd path2 && cp -dsf ../mnt/path1/file .", NULL, 0));

    cleanup();
    PASS();
#endif
}





SUITE(cp_cross_dev_symlink_suite) {
    RUN_TEST(test_cp_cross_dev_symlink);
}
DEFINE_TEST_MAIN(cp_cross_dev_symlink_suite)
