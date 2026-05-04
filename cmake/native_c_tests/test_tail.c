#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_tail_basic(void) {
    char out[512];
    FILE *f = fopen("tail_test.txt", "wb");
    fprintf(f, "line1\nline2\nline3\nline4\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("tail -n 2 tail_test.txt", out, sizeof(out)));
    char *p = out;
    char *q = out;
    while (*p) {
        if (*p != '\r') *q++ = *p;
        p++;
    }
    *q = '\0';
    
    ASSERT_STR_EQ("line3\nline4\n", out);
    remove("tail_test.txt");
    PASS();
}

TEST test_tail_sh(void) {
    char out[512];
    int ret;
    
    // tail -c pipe
    ASSERT_EQ(0, exec_capture("printf '123456' | tail -c3", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("456", out);

    // tail -c large value but valid
    ASSERT_EQ(0, exec_capture("tail -c 18446744073709551614 /dev/null 2>/dev/null", out, sizeof(out)));
    
    // Test that -c+1 outputs everything
    ASSERT_EQ(0, exec_capture("printf '123456' | tail -c+1", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("123456", out);

    // Test that -c+4 outputs from the 4th byte
    ASSERT_EQ(0, exec_capture("printf '123456' | tail -c+4", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("456", out);

    PASS();
}

TEST test_tail_basic_seek(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("file_seek", "w");
    for (int i = 0; i < 1000; i++) {
        fprintf(f, "=================================\n");
    }
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("tail -n 200 file_seek | wc -l", out, sizeof(out)));
    trim_newline(out);
    // Ignore leading spaces from wc -l
    char *p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("200", p);
    
    remove("file_seek");
#endif
    PASS();
}

TEST test_tail_start_middle(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("k_sm", "w");
    fprintf(f, "1\n2\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("(read x; tail) < k_sm", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("2", out);
    
    remove("k_sm");
#endif
    PASS();
}
TEST test_tail_append_only(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_assert_2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_assert(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_big_4gb(void) {
    char out[1024];
    
    FILE *f = fopen("big", "w");
    if (!f) { SKIPm("Could not open big file"); PASS(); }
    fwrite("abcdefgh", 1, 8, f);
    if (fseeko(f, 4294967288LL, SEEK_SET) != 0) {
        fclose(f);
        SKIPm("Could not seek to 4GB (maybe filesystem does not support sparse files)");
        PASS();
    }
    fwrite("87654321", 1, 8, f);
    fclose(f);
    
    ASSERT_EQ(0, exec_capture("tail -c1 big", out, sizeof(out)));
    ASSERT_STR_EQ("1", out);
    
    remove("big");
    PASS();
}
TEST test_tail_debug(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_descriptor_vs_rename(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_end_of_device(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_F_headers(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_F_vs_missing(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_F_vs_rename(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_flush_initial(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_follow_name(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_follow_stdin(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_dir_recreate(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_hash_abuse(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_hash_abuse2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_only_regular(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_race(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_race2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_rotate_resources(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_inotify_rotate(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_overlay_headers(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_pid_pipe(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_pid(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_pipe_f(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_pipe_f2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_proc_ksyms(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_retry(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_symlink(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_tail_c(void) {
    char out[1024];
#ifndef _WIN32
    // test pipes
    ASSERT_EQ(0, exec_capture("printf '123456' | tail -c 3", out, sizeof(out)));
    ASSERT_STR_EQ("456", out);
    
    // test /dev/zero
    struct stat st;
    if (stat("/dev/zero", &st) == 0) {
        ASSERT_EQ(0, exec_capture("head -c 10 /dev/zero > head_zero", out, sizeof(out)));
        ASSERT_EQ(0, exec_capture("tail -c 10 /dev/zero > tail_zero", out, sizeof(out)));
        ASSERT_EQ(0, exec_capture("cmp head_zero tail_zero", out, sizeof(out)));
        remove("head_zero");
        remove("tail_zero");
    }
#endif
    PASS();
}
TEST test_tail_tail_n0f(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_tail_sysfs(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_tail_pl(void) {
    char out[1024];
    FILE *f = fopen("t_pl", "w");
    fprintf(f, "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\n");
    fclose(f);
    
    ASSERT_EQ(0, exec_capture("tail -n 2 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("j\nk\n", out);
    
    ASSERT_EQ(0, exec_capture("tail -n +2 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("b\nc\nd\ne\nf\ng\nh\ni\nj\nk\n", out);
    
    ASSERT_EQ(0, exec_capture("tail -c 2 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("k\n", out);
    
    ASSERT_EQ(0, exec_capture("tail -c +2 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\n", out);
    
    ASSERT_EQ(0, exec_capture("tail -q -n 1 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("k\n", out);
    
    ASSERT_EQ(0, exec_capture("tail -v -n 1 t_pl", out, sizeof(out)));
    ASSERT_STR_EQ("==> t_pl <==\nk\n", out);
    
    my_system("rm -f t_pl");
    PASS();
}
TEST test_tail_truncate(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_wait(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_tail_tail(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}



SUITE(tail_suite) {
    RUN_TEST(test_tail_basic);
    RUN_TEST(test_tail_sh);
    RUN_TEST(test_tail_basic_seek);
    // RUN_TEST(test_tail_start_middle);
    RUN_TEST(test_tail_append_only);
    RUN_TEST(test_tail_assert_2);
    RUN_TEST(test_tail_assert);
    RUN_TEST(test_tail_big_4gb);
    RUN_TEST(test_tail_debug);
    RUN_TEST(test_tail_descriptor_vs_rename);
    RUN_TEST(test_tail_end_of_device);
    RUN_TEST(test_tail_F_headers);
    RUN_TEST(test_tail_F_vs_missing);
    RUN_TEST(test_tail_F_vs_rename);
    RUN_TEST(test_tail_flush_initial);
    RUN_TEST(test_tail_follow_name);
    RUN_TEST(test_tail_follow_stdin);
    RUN_TEST(test_tail_inotify_dir_recreate);
    RUN_TEST(test_tail_inotify_hash_abuse);
    RUN_TEST(test_tail_inotify_hash_abuse2);
    RUN_TEST(test_tail_inotify_only_regular);
    RUN_TEST(test_tail_inotify_race);
    RUN_TEST(test_tail_inotify_race2);
    RUN_TEST(test_tail_inotify_rotate_resources);
    RUN_TEST(test_tail_inotify_rotate);
    RUN_TEST(test_tail_overlay_headers);
    RUN_TEST(test_tail_pid_pipe);
    RUN_TEST(test_tail_pid);
    RUN_TEST(test_tail_pipe_f);
    RUN_TEST(test_tail_pipe_f2);
    RUN_TEST(test_tail_proc_ksyms);
    RUN_TEST(test_tail_retry);
    RUN_TEST(test_tail_symlink);
    RUN_TEST(test_tail_tail_c);
    RUN_TEST(test_tail_tail_n0f);
    RUN_TEST(test_tail_tail_sysfs);
    RUN_TEST(test_tail_tail_pl);
    RUN_TEST(test_tail_truncate);
    RUN_TEST(test_tail_wait);
    RUN_TEST(test_tail_tail);
}
DEFINE_TEST_MAIN(tail_suite)
