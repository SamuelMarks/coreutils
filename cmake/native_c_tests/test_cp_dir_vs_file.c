#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf dir file");
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

TEST test_cp_dir_vs_file(void) {
    cleanup();

    MKDIR("dir");
    create_file("file");

    // In 4.0.35, this cp invocation silently succeeded.
    ASSERT_NEQ(0, exec_capture("cp -R dir file 2>/dev/null", NULL, 0));

    // Make sure file is not replaced with a directory.
    struct stat st;
    ASSERT_EQ(0, stat("file", &st));
    ASSERT(S_ISREG(st.st_mode));

    cleanup();
    PASS();
}





SUITE(cp_dir_vs_file_suite) {
    RUN_TEST(test_cp_dir_vs_file);
}
DEFINE_TEST_MAIN(cp_dir_vs_file_suite)
