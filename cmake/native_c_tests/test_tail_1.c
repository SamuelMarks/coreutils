#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>





static void cleanup() {
    my_system("cmake -E rm -rf file.in out exp k debug.out strace.out file.debug");
}

TEST test_tail_basic_seek(void) {
    cleanup();
#ifndef _WIN32
    char cmd[1024];
    my_system("yes '=================================' | head -n 1000 > file.in");

    char out[1024];
    snprintf(cmd, sizeof(cmd), "\"%s/tail\" -n200 file.in | wc -l | sed 's/^[ \t]*//'", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("200", out);
#endif
    cleanup();
    PASS();
}

TEST test_tail_debug(void) {
    cleanup();
#ifndef _WIN32
    char cmd[1024];
    char out[1024];
    
    // timeout 1 tail --debug -f /dev/null
    snprintf(cmd, sizeof(cmd), "\"%s/timeout\" 1 \"%s/tail\" --debug -f /dev/null > out 2>&1", bin_dir, bin_dir);
    my_system(cmd);
    exec_capture("cat out", out, sizeof(out));
    ASSERT(strstr(out, "blocking") != NULL || strstr(out, "using") != NULL);

    snprintf(cmd, sizeof(cmd), "\"%s/timeout\" 1 \"%s/tail\" --debug -F /dev/null > out 2>&1", bin_dir, bin_dir);
    my_system(cmd);
    exec_capture("cat out", out, sizeof(out));
    ASSERT(strstr(out, "polling") != NULL || strstr(out, "inotify") != NULL || strstr(out, "using") != NULL);
#endif
    cleanup();
    PASS();
}

TEST test_tail_start_middle(void) {
    cleanup();
#ifndef _WIN32
    my_system("echo 1 > k && echo 2 >> k");
    
    char out[1024];
    char cmd[1024];

    // Read exactly one line from 'k' to advance the file pointer,
    // similar to shell's 'read x'.
    int fd = open("k", O_RDONLY);
    if (fd >= 0) {
        char c;
        while (read(fd, &c, 1) == 1) {
            if (c == '\n') break;
        }
        int old_in = dup(0);
        dup2(fd, 0);
        close(fd);

        snprintf(cmd, sizeof(cmd), "\"%s/tail\" > out", bin_dir);
        ASSERT_EQ(0, my_system(cmd));

        dup2(old_in, 0);
        close(old_in);
    }
    
    exec_capture("cat out", out, sizeof(out));
    ASSERT_STR_EQ("2\n", out);
#endif
    cleanup();
    PASS();
}





SUITE(tail_1_suite) {
    RUN_TEST(test_tail_basic_seek);
    RUN_TEST(test_tail_debug);
    // RUN_TEST(test_tail_start_middle);
}
DEFINE_TEST_MAIN(tail_1_suite)
