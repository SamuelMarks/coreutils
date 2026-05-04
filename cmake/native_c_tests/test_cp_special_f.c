#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f fifo e");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_special_f(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("FIFOs not supported on Windows");
#else
    if (mkfifo("fifo", 0600) != 0) {
        cleanup();
        SKIPm("mkfifo not supported");
    }

    create_file("e");

    // First time, 'e' is a regular file
    ASSERT_EQ(0, exec_capture("cp -R fifo e", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, lstat("e", &st));
    ASSERT(S_ISFIFO(st.st_mode));

    // Second time, 'e' is now a FIFO, -f should unlink and recreate it,
    // though the shell script tests `cp -R -f fifo e` in the loop.
    // The shell script loop runs with force='' and force='-f'.
    // Wait, the shell loop does:
    // for force in '' '-f'; do cp -R $force fifo e || fail=1; done
    // That means `cp -R fifo e` will overwrite regular file `e` with a fifo `e`.
    // Next time `e` is already a fifo, `cp -R -f fifo e` will unlink it and create a new fifo.
    ASSERT_EQ(0, exec_capture("cp -R -f fifo e", NULL, 0));
    
    ASSERT_EQ(0, lstat("e", &st));
    ASSERT(S_ISFIFO(st.st_mode));
#endif

    cleanup();
    PASS();
}





SUITE(cp_special_f_suite) {
    RUN_TEST(test_cp_special_f);
}
DEFINE_TEST_MAIN(cp_special_f_suite)
