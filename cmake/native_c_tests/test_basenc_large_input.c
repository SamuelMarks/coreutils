#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>



static void cleanup() {
    my_system("rm -f file.zeros out.b58");
}

TEST test_basenc_large_input(void) {
    cleanup();

    int fd = open("file.zeros", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    ASSERT(fd >= 0);
    off_t size = 20 * 1024 * 1024; // 20 MiB
#ifdef _WIN32
    lseek(fd, size - 1, SEEK_SET);
    write(fd, "\0", 1);
#else
    if (ftruncate(fd, size) != 0) {
        lseek(fd, size - 1, SEEK_SET);
        write(fd, "\0", 1);
    }
#endif
    close(fd);

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "%s/basenc --base58 file.zeros > out.b58", bin_dir);
    int res = my_system(cmd);
    
    // Check WEXITSTATUS if it was an exit code
    ASSERT_EQ(0, WIFEXITED(res) ? WEXITSTATUS(res) : -1);

    struct stat st;
    ASSERT_EQ(0, stat("out.b58", &st));
    
    // Original test: test $(basenc --base58 file.zeros | wc -c) = 21247462
    ASSERT_EQ((off_t)21247462, st.st_size);

    cleanup();
    PASS();
}





SUITE(basenc_large_input_suite) {
    RUN_TEST(test_basenc_large_input);
}
DEFINE_TEST_MAIN(basenc_large_input_suite)
