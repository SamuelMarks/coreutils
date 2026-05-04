#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>



static void cleanup() {
    my_system("rm -f k k2 cp.out");
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

TEST test_cp_sparse_2(void) {
    cleanup();

    int fd = open("k", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ASSERT(fd >= 0);
    write(fd, "x", 1);
    lseek(fd, 128 * 1024, SEEK_SET);
    write(fd, "", 0); // just truncate/create hole
    close(fd);

    const char *sparse_opts[] = {"always", "never"};

    for (int append = 0; append < 2; append++) {
        if (append == 1) {
            fd = open("k", O_WRONLY | O_APPEND);
            write(fd, "y", 1);
            close(fd);
        }

        for (int i = 0; i < 2; i++) {
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "cp --reflink=never --sparse=%s k k2", sparse_opts[i]);
            ASSERT_EQ(0, exec_capture(cmd, NULL, 0));
            ASSERT_EQ(0, compare_files("k", "k2"));
        }
    }

    // Ensure that --sparse=always can restore holes.
    unlink("k");
    fd = open("k", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ASSERT(fd >= 0);
    write(fd, "x", 1);
    char zero_buf[1024] = {0};
    for (int i = 0; i < 255; i++) {
        write(fd, zero_buf, 1024);
    }
    close(fd);

    char out[1024] = {0};
    
    // cp should detect the all-zero blocks and convert some of them to holes.
    // wait, we need to capture stdout/stderr from `cp`. exec_capture does that.
    ASSERT_EQ(0, exec_capture("cp --debug --reflink=never --sparse=always k k2", out, sizeof(out)));
    ASSERT_EQ(0, compare_files("k", "k2"));

    ASSERT(strstr(out, "sparse detection:") != NULL && strstr(out, "zeros") != NULL);

    memset(out, 0, sizeof(out));
    // cp should disable reflink AND copy offload with --sparse=never
    ASSERT_EQ(0, exec_capture("cp --debug --sparse=never k k2", out, sizeof(out)));
    ASSERT_EQ(0, compare_files("k", "k2"));
    
    ASSERT(strstr(out, "copy offload: avoided, reflink: no") != NULL);

    cleanup();
    PASS();
}





SUITE(cp_sparse_2_suite) {
    RUN_TEST(test_cp_sparse_2);
}
DEFINE_TEST_MAIN(cp_sparse_2_suite)
