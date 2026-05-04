#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    unlink("y/x/bar");
    rmdir("y/x");
    rmdir("x");
    rmdir("y");
    unlink("dst/foo/bar~");
    unlink("dst/foo/bar");
    unlink("src/foo/bar");
    rmdir("dst/foo");
    rmdir("src/foo");
    rmdir("dst");
    rmdir("src");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_backup_dir(void) {
    cleanup();

    MKDIR("x");
    MKDIR("y");

    // cp -a x y
    ASSERT_EQ(0, exec_capture("cp -a x y", NULL, 0));

    // cp -ab x y
    ASSERT_EQ(0, exec_capture("cp -ab x y", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("y/x", &st));
    ASSERT(S_ISDIR(st.st_mode));
    
    ASSERT_EQ(-1, stat("y/x~", &st));

    // Bug 62607
    MKDIR("src");
    MKDIR("src/foo");
    MKDIR("dst");
    MKDIR("dst/foo");
    create_file("src/foo/bar");
    create_file("dst/foo/bar");
    
    ASSERT_EQ(0, exec_capture("cp --recursive --backup src/foo dst", NULL, 0));

    cleanup();
    PASS();
}





SUITE(cp_backup_dir_suite) {
    RUN_TEST(test_cp_backup_dir);
}
DEFINE_TEST_MAIN(cp_backup_dir_suite)
