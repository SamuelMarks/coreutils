#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f time_check file copy file2 empty_copy");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_reflink_perm(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("permissions testing not supported on Windows");
#else
    create_file("time_check");
    create_file("file");

    if (my_system("touch -d '2009-08-28 19:00' file 2>/dev/null") != 0) {
        cleanup();
        SKIPm("touch -d not supported");
    }

    struct stat st_time, st_file;
    stat("time_check", &st_time);
    stat("file", &st_file);
    if (st_time.st_mtime <= st_file.st_mtime) {
        cleanup();
        SKIPm("The system clock is wrong");
    }

    chmod("file", 0777);
    umask(077);

    ASSERT_EQ(0, exec_capture("cp --reflink=auto --preserve file copy", NULL, 0));

    struct stat st_copy;
    ASSERT_EQ(0, stat("copy", &st_copy));
    ASSERT_EQ((mode_t)0777, st_copy.st_mode & 07777);

    ASSERT_EQ(st_file.st_mtime, st_copy.st_mtime);

    // Ensure that --attributes-only overrides --reflink completely
    FILE *f2 = fopen("file2", "w");
    if (f2) {
        fprintf(f2, "\n");
        fclose(f2);
    }

    ASSERT_EQ(0, exec_capture("cp --reflink=auto --preserve --attributes-only file2 empty_copy", NULL, 0));
    struct stat st_empty;
    ASSERT_EQ(0, stat("empty_copy", &st_empty));
    ASSERT_EQ(0, st_empty.st_size);

    unlink("empty_copy");

    // --reflink=always might fail on non-reflink filesystems, 
    // but with --attributes-only it should ignore reflink.
    ASSERT_EQ(0, exec_capture("cp --reflink=always --preserve --attributes-only file2 empty_copy", NULL, 0));
    ASSERT_EQ(0, stat("empty_copy", &st_empty));
    ASSERT_EQ(0, st_empty.st_size);

#endif

    cleanup();
    PASS();
}





SUITE(cp_reflink_perm_suite) {
    RUN_TEST(test_cp_reflink_perm);
}
DEFINE_TEST_MAIN(cp_reflink_perm_suite)
