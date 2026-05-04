#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



static void cleanup() {
    unlink("j");
    unlink("j2");
}

static int compare_files(const char *f1, const char *f2) {
    char buf1[4096], buf2[4096];
    FILE *file1 = fopen(f1, "rb");
    FILE *file2 = fopen(f2, "rb");
    if (!file1 || !file2) {
        if (file1) fclose(file1);
        if (file2) fclose(file2);
        return -1;
    }
    while (1) {
        size_t n1 = fread(buf1, 1, sizeof(buf1), file1);
        size_t n2 = fread(buf2, 1, sizeof(buf2), file2);
        if (n1 != n2) {
            fclose(file1);
            fclose(file2);
            return -1;
        }
        if (n1 == 0) break;
        if (memcmp(buf1, buf2, n1) != 0) {
            fclose(file1);
            fclose(file2);
            return -1;
        }
    }
    fclose(file1);
    fclose(file2);
    return 0;
}

TEST test_cp_copy_FMR(void) {
    cleanup();

    int fd = open("j", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    ASSERT(fd >= 0);
    
    char a1024[1024];
    memset(a1024, 'a', sizeof(a1024));

    for (int i = 0; i < 600; i++) {
        ASSERT_NEQ((off_t)-1, lseek(fd, 4096, SEEK_CUR));
        ASSERT_EQ(1024, write(fd, a1024, 1024));
    }
    close(fd);

    // Instead of explicitly running under valgrind (which may not be installed),
    // we just run the cp command. If built with ASAN, it will catch FMR.
    int res = exec_capture("cp --reflink=never j j2", NULL, 0);
    ASSERT_EQ(0, res);

    ASSERT_EQ(0, compare_files("j", "j2"));

    cleanup();
    PASS();
}





SUITE(cp_copy_FMR_suite) {
    RUN_TEST(test_cp_copy_FMR);
}
DEFINE_TEST_MAIN(cp_copy_FMR_suite)
