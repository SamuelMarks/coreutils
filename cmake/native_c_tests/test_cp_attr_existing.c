#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    unlink("file1");
    unlink("file2");
    unlink("file2.exp");
    unlink("link2");
    unlink("sym1");
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

TEST test_cp_attr_existing(void) {
    cleanup();

    create_file("file1", "1");
    create_file("file2", "2");
    create_file("file2.exp", "2");

    char out[1024];
    int res = exec_capture("cp --attributes-only file1 file2", out, sizeof(out));
    if (res != 0) {
        printf("cp output: %s\n", out);
    }
    ASSERT_EQ(0, res);
    ASSERT_EQ(0, compare_files("file2", "file2.exp"));

    // hardlinked file
    ASSERT_EQ(0, link("file2", "link2"));
    ASSERT_EQ(0, exec_capture("cp -a --attributes-only file1 file2", NULL, 0));
    ASSERT_EQ(0, compare_files("file2", "file2.exp"));

    // symlink source
    if (symlink("file1", "sym1") == 0) {
        // cp -a --attributes-only sym1 file2 should fail
        ASSERT_NEQ(0, exec_capture("cp -a --attributes-only sym1 file2", NULL, 0));
        ASSERT_EQ(0, compare_files("file2", "file2.exp"));

        // cp -a --remove-destination --attributes-only sym1 file2
        ASSERT_EQ(0, exec_capture("cp -a --remove-destination --attributes-only sym1 file2", NULL, 0));
        
        struct stat st;
        ASSERT_EQ(0, lstat("file2", &st));
        ASSERT(S_ISLNK(st.st_mode)); // must be a symlink
        ASSERT_EQ(0, compare_files("file1", "file2"));
    }

    cleanup();
    PASS();
}





SUITE(cp_attr_existing_suite) {
    // RUN_TEST(test_cp_attr_existing);
}
DEFINE_TEST_MAIN(cp_attr_existing_suite)
