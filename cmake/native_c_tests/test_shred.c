#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>




TEST test_shred_basic(void) {
    FILE *f = fopen("test_shred.txt", "w");
    fprintf(f, "sensitive data\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("shred -u test_shred.txt", NULL, 0));
    struct stat st;
    ASSERT(stat("test_shred.txt", &st) != 0); /* File should be removed */
    PASS();
}

TEST test_shred_exact(void) {
    FILE *f = fopen("a", "w");
    fprintf(f, "a\n");
    if (f) fclose(f);
    f = fopen("b", "w");
    fprintf(f, "bb\n");
    if (f) fclose(f);
    f = fopen("c", "w");
    fprintf(f, "ccc\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("shred --remove --exact a b", NULL, 0));
    struct stat st;
    ASSERT(stat("a", &st) != 0);
    ASSERT(stat("b", &st) != 0);
    
    ASSERT_EQ(0, exec_capture("shred --remove --zero c", NULL, 0));
    ASSERT(stat("c", &st) != 0);

    PASS();
}

TEST test_shred_passes(void) {
    FILE *f = fopen("d", "w");
    fprintf(f, "d\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("shred -n 1 -u d", out, sizeof(out)));
    struct stat st;
    ASSERT(stat("d", &st) != 0);
    PASS();
}

TEST test_shred_size(void) {
    FILE *f = fopen("e", "w");
    fprintf(f, "e\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("shred -s 10 -u e", NULL, 0));
    struct stat st;
    ASSERT(stat("e", &st) != 0);
    PASS();
}

TEST test_shred_remove(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        FILE *f = fopen("0123456789", "w"); if (f) fclose(f);
        chmod("0123456789", 0400);
        
        { int _r = exec_capture("shred -u 0123456789 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        remove("0123456789");
        
        // test remove methods
        f = fopen("shred_file", "w"); if (f) fclose(f);
        ASSERT_EQ(0, exec_capture("shred -n0 --remove=wipe shred_file", out, sizeof(out)));
        struct stat st;
        ASSERT_EQ(-1, stat("shred_file", &st));
    }
#endif
    PASS();
}



SUITE(shred_suite) {
    RUN_TEST(test_shred_basic);
    RUN_TEST(test_shred_exact);
    RUN_TEST(test_shred_passes);
    RUN_TEST(test_shred_size);
    RUN_TEST(test_shred_remove);
}
DEFINE_TEST_MAIN(shred_suite)
