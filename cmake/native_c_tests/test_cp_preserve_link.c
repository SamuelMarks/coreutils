#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf s t");
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

static int same_inode(const char *f1, const char *f2) {
    struct stat st1, st2;
    if (stat(f1, &st1) != 0) return 0;
    if (stat(f2, &st2) != 0) return 0;
    return st1.st_ino == st2.st_ino;
}

static void create_source_tree() {
    my_system("cmake -E rm -rf s");
    MKDIR("s");
    create_file("s/f");
    link("s/f", "s/linkm");
    link("s/f", "s/linke");
    link("s/f", "s/fileo");
    link("s/f", "s/fileu");
}

static void create_target_tree(const char *f) {
    my_system("cmake -E rm -rf t");
    my_system("mkdir -p t/s/");
    char path[256];
    snprintf(path, sizeof(path), "t/s/%s", f);
    create_file(path);
    
    char linke[256];
    snprintf(linke, sizeof(linke), "t/s/linke");
    link(path, linke);
    
    // a separate older file in dest
    create_file("t/s/fileo");
    my_system("touch -d '-1 hour' t/s/fileo 2>/dev/null");
    
    // a separate newer file in dest
    create_file("t/s/fileu");
    my_system("touch -d '+1 hour' t/s/fileu 2>/dev/null");
}

TEST test_cp_preserve_link(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("hardlinks testing not supported on Windows");
#else
    create_file("dummy");
    if (link("dummy", "dummy_link") != 0) {
        unlink("dummy");
        cleanup();
        SKIPm("hard links not supported");
    }
    unlink("dummy");
    unlink("dummy_link");

    const char *fs[] = {"f", "linkm"};
    for (int i = 0; i < 2; i++) {
        create_source_tree();
        create_target_tree(fs[i]);

        ASSERT_EQ(0, exec_capture("cp -au s t", NULL, 0));

        ASSERT(same_inode("t/s/f", "t/s/linkm"));
        ASSERT(same_inode("t/s/f", "t/s/linke"));
        ASSERT(same_inode("t/s/f", "t/s/fileo"));
        ASSERT(same_inode("t/s/f", "t/s/fileu"));
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_preserve_link_suite) {
    RUN_TEST(test_cp_preserve_link);
}
DEFINE_TEST_MAIN(cp_preserve_link_suite)
