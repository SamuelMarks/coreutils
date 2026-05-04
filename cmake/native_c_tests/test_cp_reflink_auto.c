#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f a_other b");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) {
        fprintf(f, "non_zero_size\n");
        if (f) fclose(f);
    }
}

TEST test_cp_reflink_auto(void) {
    cleanup();

    // The shell test requires another partition (other-fs-tmpdir) to ensure
    // that reflink fails. We can simulate a failure case by using a known
    // path, but cross-device checking is hard to automate safely in C without
    // explicit mount points setup (which requires root/mocking).
    // Let's test the basic syntax/parsing logic at minimum.

    create_file("a_other");

    // --reflink=auto should fall back to a normal copy
    ASSERT_EQ(0, exec_capture("cp --reflink=auto a_other b", NULL, 0));
    struct stat st;
    ASSERT_EQ(0, stat("b", &st));
    ASSERT(st.st_size > 0);

    unlink("b");

    // --reflink=auto should allow --sparse for fallback copies.
    ASSERT_EQ(0, exec_capture("cp --reflink=auto --sparse=always a_other b", NULL, 0));
    ASSERT_EQ(0, stat("b", &st));
    ASSERT(st.st_size > 0);

    unlink("b");

    // --reflink=auto should be overridden by --reflink=never
    ASSERT_EQ(0, exec_capture("cp --reflink=auto --reflink=never a_other b", NULL, 0));
    ASSERT_EQ(0, stat("b", &st));
    ASSERT(st.st_size > 0);

    cleanup();
    PASS();
}





SUITE(cp_reflink_auto_suite) {
    RUN_TEST(test_cp_reflink_auto);
}
DEFINE_TEST_MAIN(cp_reflink_auto_suite)
