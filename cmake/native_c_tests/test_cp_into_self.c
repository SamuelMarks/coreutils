#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a dir out exp");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_into_self(void) {
    cleanup();

    MKDIR("a");
    MKDIR("dir");

    char out[1024];

    // cp -R dir dir
    ASSERT_NEQ(0, exec_capture("cp -R dir dir 2>&1", out, sizeof(out)));
    ASSERT(strstr(out, "into itself") != NULL);

    // cp -rl dir dir
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp -rl dir dir 2>&1", out, sizeof(out)));
    ASSERT(strstr(out, "into itself") != NULL);

    // cp -rl a dir dir
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp -rl a dir dir 2>&1", out, sizeof(out)));
    ASSERT(strstr(out, "into itself") != NULL);

    // run it again
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp -rl a dir dir 2>&1", out, sizeof(out)));
    ASSERT(strstr(out, "into itself") != NULL);

    cleanup();
    PASS();
}





SUITE(cp_into_self_suite) {
    RUN_TEST(test_cp_into_self);
}
DEFINE_TEST_MAIN(cp_into_self_suite)
