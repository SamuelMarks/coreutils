#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    unlink("src_file");
    unlink("dest_file");
    unlink("dest2_file");
    unlink("dummy");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_link(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("links testing not supported on Windows");
#else
    create_file("src_file");
    create_file("dest_file");
    create_file("dest2_file");

    char out[1024] = {0};
    int res = exec_capture("cp -f --link src_file dest_file", out, sizeof(out));
    if (res != 0) {
        printf("Output from cp -f --link src_file dest_file: %s\n", out);
    }
    ASSERT_EQ(0, res);
    
    if (symlink("src_file", "dummy") == 0) {
        unlink("dummy");
        ASSERT_EQ(0, exec_capture("cp -f --symbolic-link src_file dest2_file", NULL, 0));
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_link_suite) {
    RUN_TEST(test_cp_link);
}
DEFINE_TEST_MAIN(cp_link_suite)
