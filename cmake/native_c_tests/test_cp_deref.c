#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    my_system("cmake -E rm -rf a b c d");
}

TEST test_cp_deref(void) {
    cleanup();

#ifdef _WIN32
    mkdir("a");
    mkdir("b");
    mkdir("c");
    mkdir("d");
#else
    mkdir("a", 0700);
    mkdir("b", 0700);
    mkdir("c", 0700);
    mkdir("d", 0700);
#endif

    if (symlink("../c", "a/c") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }
    
    if (symlink("../c", "b/c") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }

    ASSERT_EQ(0, exec_capture("cp -RL a b d", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, stat("d/a/c", &st));
    ASSERT(S_ISDIR(st.st_mode));

    ASSERT_EQ(0, stat("d/b/c", &st));
    ASSERT(S_ISDIR(st.st_mode));

    cleanup();
    PASS();
}





SUITE(cp_deref_suite) {
    RUN_TEST(test_cp_deref);
}
DEFINE_TEST_MAIN(cp_deref_suite)
