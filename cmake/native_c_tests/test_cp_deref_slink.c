#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    unlink("f");
    unlink("slink-target");
    unlink("slink");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_deref_slink(void) {
    cleanup();

    create_file("f");
    create_file("slink-target");

#ifndef _WIN32
    if (symlink("slink-target", "slink") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }
#endif

    // cp -d f slink
    ASSERT_EQ(0, exec_capture("cp -d f slink", NULL, 0));

    cleanup();
    PASS();
}





SUITE(cp_deref_slink_suite) {
    RUN_TEST(test_cp_deref_slink);
}
DEFINE_TEST_MAIN(cp_deref_slink_suite)
