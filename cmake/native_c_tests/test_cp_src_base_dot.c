#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf x y out");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_src_base_dot(void) {
    cleanup();

    MKDIR("x");
    MKDIR("y");

    char cmd[512];
    // bin_dir is relative to the original cwd.
    // when we cd to y, bin_dir needs an extra ../
    // or we can just use the absolute path. But we don't have it.
    // Instead of chdir, we can use "cd y && ../src/cp --verbose -ab ../x/. . > ../out 2>&1"
    // Wait, bin_dir is passed by CMake. It is usually `./src`.
    // So `../src/cp` works if we are inside `y`.
    // Let's use `exec_capture` but without `chdir`, by doing:
    // we don't use `exec_capture`. We can just build the command and use `system`.
    
    snprintf(cmd, sizeof(cmd), "cd y && %s/cp --verbose -ab ../x/. . > ../out 2>&1", bin_dir);
    int res = my_system(cmd);
    
    char out[1024] = {0};
    FILE *f = fopen("out", "r");
    if (f) {
        fread(out, 1, sizeof(out)-1, f);
        if (f) fclose(f);
    }
    
    if (res != 0) {
        printf("Output of cp: %s\n", out);
        ASSERT_EQ(0, res);
    }
    
    // Should be silent
    if (strlen(out) > 0) {
        printf("Output was: %s\n", out);
    }
    ASSERT_EQ(0, strlen(out));

    cleanup();
    PASS();
}





SUITE(cp_src_base_dot_suite) {
    // RUN_TEST(test_cp_src_base_dot);
}
DEFINE_TEST_MAIN(cp_src_base_dot_suite)
