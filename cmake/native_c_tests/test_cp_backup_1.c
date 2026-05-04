#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    unlink("F");
    unlink("F.b");
}

static void create_file(const char *name, const char *content) {
    FILE *f = fopen(name, "w");
    if (f) {
        fprintf(f, "%s", content);
        if (f) fclose(f);
    }
}

static int compare_files(const char *f1, const char *f2) {
    char buf1[64] = {0}, buf2[64] = {0};
    FILE *file1 = fopen(f1, "r");
    FILE *file2 = fopen(f2, "r");
    if (!file1 || !file2) {
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return -1;
    }
    size_t n1 = fread(buf1, 1, sizeof(buf1)-1, file1);
    size_t n2 = fread(buf2, 1, sizeof(buf2)-1, file2);
    fclose(file1);
    fclose(file2);
    if (n1 != n2) return -1;
    return strcmp(buf1, buf2);
}

TEST test_cp_backup_1(void) {
    cleanup();

    create_file("F", "test\n");

    // cp --force --backup=simple --suffix=.b F F
    ASSERT_EQ(0, exec_capture("cp --force --backup=simple --suffix=.b F F", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("F", &st));
    ASSERT_EQ(0, stat("F.b", &st));
    ASSERT_EQ(0, compare_files("F", "F.b"));

    unlink("F.b");

    // cp -T --force --backup=simple --suffix=.b F F
    ASSERT_EQ(0, exec_capture("cp -T --force --backup=simple --suffix=.b F F", NULL, 0));
    
    ASSERT_EQ(0, stat("F", &st));
    ASSERT_EQ(0, stat("F.b", &st));
    ASSERT_EQ(0, compare_files("F", "F.b"));

    cleanup();
    PASS();
}





SUITE(cp_backup_1_suite) {
    RUN_TEST(test_cp_backup_1);
}
DEFINE_TEST_MAIN(cp_backup_1_suite)
