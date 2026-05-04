#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf target \"\xff\xff_dir\"");
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_non_utf8_name(void) {
    cleanup();

    const char *non_utf8_dir = "\xff\xff_dir";

    if (MKDIR(non_utf8_dir) != 0) {
        SKIPm("bad unicode not supported on file system");
    }
    MKDIR("target");

    char file1[256], file2[256];
    snprintf(file1, sizeof(file1), "%s/file1", non_utf8_dir);
    snprintf(file2, sizeof(file2), "%s/file2", non_utf8_dir);

    FILE *f1 = fopen(file1, "w");
    if (f1) fclose(f1);
    FILE *f2 = fopen(file2, "w");
    if (f2) fclose(f2);

    char cmd[256];
    // cp -r "\xff\xff_dir"/. target
    snprintf(cmd, sizeof(cmd), "cp -r \"%s\"/. target", non_utf8_dir);
    
    ASSERT_EQ(0, exec_capture(cmd, NULL, 0));

    struct stat st;
    ASSERT_EQ(0, stat("target/file1", &st));
    ASSERT_EQ(0, stat("target/file2", &st));
    
    ASSERT_EQ(0, stat(file1, &st));
    ASSERT_EQ(0, stat(file2, &st));

    cleanup();
    PASS();
}





SUITE(cp_non_utf8_name_suite) {
    RUN_TEST(test_cp_non_utf8_name);
}
DEFINE_TEST_MAIN(cp_non_utf8_name_suite)
