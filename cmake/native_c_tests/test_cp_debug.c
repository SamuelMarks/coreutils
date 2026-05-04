#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    unlink("file");
    unlink("file.cp");
    unlink("file.cp2");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_debug(void) {
    cleanup();

    create_file("file");

    char out[1024] = {0};

    // cp --debug file file.cp
    ASSERT_EQ(0, exec_capture("cp --debug file file.cp", out, sizeof(out)));

    // Check for standard debug output string
    ASSERT(strstr(out, "copy offload:") != NULL);
    ASSERT(strstr(out, "reflink:") != NULL);
    ASSERT(strstr(out, "sparse detection:") != NULL);

    // cp --debug --attributes-only file file.cp
    memset(out, 0, sizeof(out));
    ASSERT_EQ(0, exec_capture("cp --debug --attributes-only file file.cp", out, sizeof(out)));
    ASSERT(strstr(out, "copy offload:") == NULL);

    // cp --debug --update=none file file.cp
    create_file("file.cp");
    memset(out, 0, sizeof(out));
    ASSERT_EQ(0, exec_capture("cp --debug --update=none file file.cp", out, sizeof(out)));
    ASSERT(strstr(out, "skipped") != NULL);

#ifndef _WIN32
    // test /dev/full
    struct stat st;
    if (stat("/dev/full", &st) == 0 && S_ISCHR(st.st_mode) && access("/dev/full", W_OK) == 0) {
        ASSERT_NEQ(0, exec_capture("cp file file.cp2 --debug >/dev/full 2>/dev/null", NULL, 0));
        ASSERT_EQ(0, stat("file.cp2", &st));
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_debug_suite) {
    RUN_TEST(test_cp_debug);
}
DEFINE_TEST_MAIN(cp_debug_suite)
