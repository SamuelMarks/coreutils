#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>



static void cleanup() {
    my_system("rm -f sparse copy file.in sparse.out sparse.out2 zeros nonzero");
}

static int compare_files(const char *f1, const char *f2) {
    char buf1[8192], buf2[8192];
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

TEST test_cp_sparse(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("sparse files testing not supported on Windows");
#else
    off_t size = 128 * 1024 + 1;
    int fd = open("sparse", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ASSERT(fd >= 0);
    lseek(fd, size - 1, SEEK_SET);
    write(fd, "", 1);
    close(fd);

    ASSERT_EQ(0, exec_capture("cp --reflink=never --sparse=always sparse copy", NULL, 0));

    struct stat st_sparse, st_copy;
    ASSERT_EQ(0, stat("sparse", &st_sparse));
    ASSERT_EQ(0, stat("copy", &st_copy));

    // Ensure the copy has <= block count as original
    ASSERT(st_copy.st_blocks <= st_sparse.st_blocks);

    // Ensure that --sparse={always,never} with --reflink fail
    ASSERT_NEQ(0, exec_capture("cp --sparse=always --reflink sparse copy 2>/dev/null", NULL, 0));
    ASSERT_NEQ(0, exec_capture("cp --sparse=never --reflink sparse copy 2>/dev/null", NULL, 0));

    // Ensure we handle sparse/non-sparse transitions correctly
    int hole_size = st_copy.st_blksize;
    if (hole_size <= 0) hole_size = 4096;

    int maxn = 16; // reduced for test speed

    char *zeros = calloc(1, hole_size);
    char *nonzero = malloc(hole_size);
    memset(nonzero, 'U', hole_size);

    for (int pat = 0; pat < 2; pat++) {
        int ns[] = {1, 2, 4, 16};
        for (int n_idx = 0; n_idx < 4; n_idx++) {
            int n = ns[n_idx];
            int parts = maxn / n;
            
            unlink("file.in");
            fd = open("file.in", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            
            for (int i = 0; i < parts; i++) {
                int is_nonzero = (pat == 0) ? (i % 2 == 0) : (i % 2 != 0);
                char *buf = is_nonzero ? nonzero : zeros;
                for (int j = 0; j < n; j++) {
                    write(fd, buf, hole_size);
                }
            }
            close(fd);

            ASSERT_EQ(0, exec_capture("cp --reflink=never --sparse=always file.in sparse.out", NULL, 0));
            ASSERT_EQ(0, exec_capture("cp --reflink=never --sparse=always sparse.out sparse.out2", NULL, 0));

            ASSERT_EQ(0, compare_files("file.in", "sparse.out"));
            ASSERT_EQ(0, compare_files("file.in", "sparse.out2"));
        }
    }
    
    free(zeros);
    free(nonzero);
#endif

    cleanup();
    PASS();
}





SUITE(cp_sparse_suite) {
    RUN_TEST(test_cp_sparse);
}
DEFINE_TEST_MAIN(cp_sparse_suite)
