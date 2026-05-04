#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



static void cleanup() {
    chmod("D", 0700);
    chmod("DD", 0700);
    chmod("D/D", 0700);
    chmod("DD/D", 0700);
    my_system("chmod -R 0700 D DD 2>/dev/null"); // Make sure we can delete everything
    my_system("cmake -E rm -rf D DD symlink F out exp");
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

TEST test_cp_fail_perm(void) {
    cleanup();

#ifndef _WIN32
    if (geteuid() == 0) {
        SKIPm("must not be run as root");
    }

    MKDIR("D");
    MKDIR("D/D");
    create_file("D/a");
    chmod("D/a", 0);
    chmod("D", S_IRUSR | S_IXUSR); // rx

    char out[1024] = {0};
    int res = exec_capture("cp -pR D DD", out, sizeof(out));
    if (res == 0) {
        printf("cp output: %s\n", out);
    }
    ASSERT_NEQ(0, res);

    struct stat st;
    ASSERT_EQ(0, stat("DD", &st));
    
    ASSERT_EQ((mode_t)(S_IRUSR | S_IXUSR), st.st_mode & 07777);

    chmod("D", 0);
    symlink("D/D", "symlink");
    create_file("F");

    // cp F symlink
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp F symlink", out, sizeof(out)));
    if (strstr(out, "cannot stat 'symlink'") == NULL) {
        printf("cp output was: %s\n", out);
        ASSERT(0);
    }

    // cp --no-target-directory F symlink
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp --no-target-directory F symlink", out, sizeof(out)));
    ASSERT(strstr(out, "cannot stat 'symlink'") != NULL);

    // cp --target-directory=symlink F
    memset(out, 0, sizeof(out));
    ASSERT_NEQ(0, exec_capture("cp --target-directory=symlink F", out, sizeof(out)));
    ASSERT(strstr(out, "target directory 'symlink'") != NULL);

    chmod("D", 0700);
#else
    SKIPm("permissions testing not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_fail_perm_suite) {
    RUN_TEST(test_cp_fail_perm);
}
DEFINE_TEST_MAIN(cp_fail_perm_suite)
