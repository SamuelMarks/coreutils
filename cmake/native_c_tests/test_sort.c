#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_sort_basic(void) {
    FILE *f = fopen("test_sort.txt", "w");
    fprintf(f, "b\na\nc\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sort test_sort.txt", out, sizeof(out)));
    ASSERT(strstr(out, "a") != NULL && strstr(out, "b") != NULL && strstr(out, "c") != NULL);
    remove("test_sort.txt");
    PASS();
}

TEST test_sort_sh(void) {
    char out1[1024];
    char out2[1024];
    
    // rand
    FILE *f = fopen("in", "w");
    for (int i = 1; i <= 100; i++) {
        fprintf(f, "%d\n", i);
    }
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort -R in", out1, sizeof(out1)));
    
    f = fopen("in", "r");
    size_t n = fread(out2, 1, sizeof(out2) - 1, f);
    if (f) fclose(f);
    out2[n] = '\0';
    
    ASSERT(strcmp(out1, out2) != 0); // probabilistic

    // check permutation
    ASSERT_EQ(0, exec_capture("sort -R in | sort -n", out1, sizeof(out1)));
    ASSERT(strcmp(out1, out2) == 0); // Need sorting in same order

    remove("in");
    
    // month
    f = fopen("in_m", "w");
    fprintf(f, "FEB\nJAN\nMAR\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort -M in_m", out1, sizeof(out1)));
    // Expect JAN FEB MAR
    char out_norm[1024] = {0};
    int j = 0;
    for (int i = 0; out1[i]; i++) {
        if (out1[i] != '\r') out_norm[j++] = out1[i];
    }
    ASSERT_STR_EQ("JAN\nFEB\nMAR\n", out_norm);
    
    remove("in_m");

    PASS();
}

TEST test_sort_compress(void) {
    char out[1024];
#ifndef _WIN32
    // test compress program overriding
    FILE *f = fopen("in_c", "w");
    fprintf(f, "2\n1\n3\n");
    if (f) fclose(f);
    
    // We just test `--compress-program` flags when using a dummy program that works.
    ASSERT_EQ(0, exec_capture("sort -S 1k --compress-program=cat in_c", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n3\n", out);
    
    // Test missing program
    { int _r = exec_capture("sort -S 1k --compress-program=missing_gzip in_c 2>/dev/null", out, sizeof(out)); ASSERT_EQ(0, WEXITSTATUS(_r)); }
    
    my_system("rm -f in_c");
#endif
    PASS();
}

TEST test_sort_float(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_f", "w");
    fprintf(f, "1e1\n10\n");
    if (f) fclose(f);
    
    // General float sorting equality
    ASSERT_EQ(0, exec_capture("sort -gu in_f | wc -l", out, sizeof(out)));
    trim_newline(out);
    char *p = out;
    while (*p == ' ' || *p == '\t') p++;
    ASSERT_STR_EQ("1", p); // 1e1 and 10 should be equal
    
    remove("in_f");
#endif
    PASS();
}
TEST test_sort_exit_early(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        FILE *f = fopen("input_see", "w"); if (f) fclose(f);
        chmod("input_see", 0400); // read only
        
        { int _r = exec_capture("sort -o input_see input_see 2>/dev/null", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
        
        FILE *f2 = fopen("output_see", "w"); fclose(f2);
        chmod("output_see", 0000); // unreadable
        { int _r = exec_capture("sort -o typo_see output_see 2>/dev/null", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
        struct stat st;
        ASSERT_EQ(-1, stat("typo_see", &st)); // should not be created
        
        chmod("output_see", 0700);
        my_system("rm -f input_see output_see typo_see");
    }
#endif
    PASS();
}

TEST test_sort_unique(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_su", "w");
    fprintf(f, "1\n2\n1\n3\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort -u in_su", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n3\n", out);
    
    remove("in_su");
#endif
    PASS();
}
TEST test_sort_benchmark_random(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_compress_hang(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_compress_proc(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_continue(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_debug_keys(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_debug_warn(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_discrim(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_field_limit(void) {
    char out[1024];
    FILE *f = fopen("in", "w");
    fprintf(f, "aa\nbb\n");
    fclose(f);
    
    setenv("_POSIX2_VERSION", "200809", 1);
    
    ASSERT_EQ(0, exec_capture("sort +0.18446744073709551615R in", out, sizeof(out)));
    ASSERT_STR_EQ("aa\nbb\n", out);
    
    ASSERT_EQ(0, exec_capture("sort +1 -1.18446744073709551615R in", out, sizeof(out)));
    ASSERT_STR_EQ("aa\nbb\n", out);
    
    unsetenv("_POSIX2_VERSION");
    my_system("rm -f in");
    PASS();
}
TEST test_sort_h_thousands_sep(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_locale(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_merge_fdlimit(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_NaN_infloop(void) {
    char out[1024];
    FILE *f = fopen("F", "w");
    fprintf(f, "nan\n");
    fclose(f);

    ASSERT_EQ(0, exec_capture("sort -g -m F F", out, sizeof(out)));
    ASSERT_STR_EQ("nan\nnan\n", out);

    ASSERT_EQ(0, exec_capture("yes nan | head -n1000 | sort -g > /dev/null", out, sizeof(out)));

    my_system("rm -f F");
    PASS();
}

TEST test_sort_rand(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_spinlock_abuse(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_stale_thread_mem(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_sort_u_FMR(void) {
    char out[2048];
    FILE *f = fopen("in", "w");
    fprintf(f, "0\n");
    for (int i=0; i<900; i++) fprintf(f, "0");
    fprintf(f, "1\n");
    fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort --p=1 -S32b -u in > out_file", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cmp in out_file", out, sizeof(out)));
    my_system("rm -f in exp out_file");
    PASS();
}
TEST test_sort_unique_segv(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in", "w");
    fprintf(f, "\n\n\n\n\n\n\nz\nzzzzzz\nzzzzzzz\nzzzzzzz\nzzzzzzz\nzzzzzzzzz\nzzzzzzzzzzz\nzzzzzzzzzzzz\n");
    fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort --parallel=1 -u in > exp", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("sort --parallel=2 -u -S 10b < in > out", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cmp exp out", out, sizeof(out)));
    
    my_system("rm -f in exp out");
#endif
    PASS();
}

TEST test_sort_version(void) {
    char out[1024];
    FILE *f = fopen("in", "w");
    fprintf(f, "a-10.tar.gz\na-2.tar.gz\n");
    fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort --stable --sort=version in", out, sizeof(out)));
    ASSERT_STR_EQ("a-2.tar.gz\na-10.tar.gz\n", out);
    
    my_system("rm -f in");
    PASS();
}
TEST test_sort_sort_files0_from(void) {
    char out[1024];
#ifndef _WIN32
    { int _r = exec_capture("sort --files0-from=- no-such 2>err </dev/null", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'extra operand' err", out, sizeof(out)));
    
    { int _r = exec_capture("sort --files0-from=missing_file 2>err", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
    int r = exec_capture("grep 'No such file or directory' err", out, sizeof(out));
    if (r != 0) {
        exec_capture("cat err", out, sizeof(out));
        printf("err: %s\n", out);
    }
    ASSERT_EQ(0, r);
    
    { int _r = exec_capture("env printf '-' | sort --files0-from=- 2>err", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'no file name of' err", out, sizeof(out)));
    
    { int _r = exec_capture("sort --files0-from=/dev/null 2>err", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'no input from' err", out, sizeof(out)));
    
    // NUL testing
    FILE *f = fopen("f0_in", "w"); fwrite("\0", 1, 1, f); if (f) fclose(f);
    { int _r = exec_capture("sort --files0-from=f0_in 2>err", out, sizeof(out)); ASSERT_EQ(2, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'invalid zero-length file name' err", out, sizeof(out)));
    
    FILE *g = fopen("g", "w"); fprintf(g, "a\n"); fclose(g);
    f = fopen("f0_in", "w"); fwrite("g\0", 1, 2, f); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("sort --files0-from=f0_in", out, sizeof(out)));
    ASSERT_STR_EQ("a\n", out);
    
    f = fopen("f0_in", "w"); fwrite("g\0g\0", 1, 4, f); if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("sort --files0-from=f0_in", out, sizeof(out)));
    ASSERT_STR_EQ("a\na\n", out);
    
    remove("f0_in"); remove("g"); remove("err");
#endif
    PASS();
}

TEST test_sort_sort_merge(void) {
    char out[1024];
    my_system("touch empty1 empty2 empty3");
    my_system("echo foo > foo");
    
    int r1 = exec_capture("sort -m empty1 empty2 empty3 foo 2>err", out, sizeof(out));
    if (r1 != 0) {
        exec_capture("cat err", out, sizeof(out));
        printf("sort merge err: %s\n", out);
    }
    ASSERT_EQ(0, r1);
    ASSERT_STR_EQ("foo\n", out);
    
    int r = exec_capture("sort -m --batch-size=0 empty1 empty2 empty3 foo 2>err", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));
    ASSERT_EQ(0, exec_capture("grep 'invalid --batch-size' err", out, sizeof(out)));
    
    r = exec_capture("sort -m --batch-size=4 -Tdoes/not/exist empty1 empty2 empty3 foo", out, sizeof(out));
    ASSERT_EQ(0, WEXITSTATUS(r));
    
    r = exec_capture("sort -m --batch-size=2 -Tdoes/not/exist empty1 empty2 empty3 foo 2>err", out, sizeof(out));
    ASSERT_EQ(2, WEXITSTATUS(r));
    ASSERT_EQ(0, exec_capture("grep 'cannot create temporary file' err", out, sizeof(out)));
    
    my_system("rm -f empty1 empty2 empty3 foo err");
    PASS();
}

TEST test_sort_sort_month(void) {
    char out[1024];
#ifndef _WIN32
    // Basic POSIX/C month sort
    FILE *f = fopen("f_month", "w");
    fprintf(f, "Feb\nJan\nMar\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort -M f_month", out, sizeof(out)));
    ASSERT_STR_EQ("Jan\nFeb\nMar\n", out);
    
    remove("f_month");
#endif
    PASS();
}

TEST test_sort_sort(void) {
    char out[1024];
#ifndef _WIN32
    // Basic sorting functionality
    FILE *f = fopen("f_sort", "w");
    fprintf(f, "c\nb\na\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("sort f_sort", out, sizeof(out)));
    ASSERT_STR_EQ("a\nb\nc\n", out);
    
    // Reverse
    ASSERT_EQ(0, exec_capture("sort -r f_sort", out, sizeof(out)));
    ASSERT_STR_EQ("c\nb\na\n", out);
    
    // Numeric
    f = fopen("f_sort", "w");
    fprintf(f, "10\n2\n1\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("sort -n f_sort", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n10\n", out);
    
    // Key
    f = fopen("f_sort", "w");
    fprintf(f, "2 c\n1 b\n3 a\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("sort -k 2 f_sort", out, sizeof(out)));
    ASSERT_STR_EQ("3 a\n1 b\n2 c\n", out);
    
    remove("f_sort");
#endif
    PASS();
}



SUITE(sort_suite) {
    RUN_TEST(test_sort_basic);
    RUN_TEST(test_sort_sh);
    RUN_TEST(test_sort_compress);
    RUN_TEST(test_sort_float);
    RUN_TEST(test_sort_exit_early);
    RUN_TEST(test_sort_unique);
    RUN_TEST(test_sort_benchmark_random);
    RUN_TEST(test_sort_compress_hang);
    RUN_TEST(test_sort_compress_proc);
    RUN_TEST(test_sort_continue);
    RUN_TEST(test_sort_debug_keys);
    RUN_TEST(test_sort_debug_warn);
    RUN_TEST(test_sort_discrim);
    RUN_TEST(test_sort_field_limit);
    RUN_TEST(test_sort_h_thousands_sep);
    RUN_TEST(test_sort_locale);
    RUN_TEST(test_sort_merge_fdlimit);
    RUN_TEST(test_sort_NaN_infloop);
    RUN_TEST(test_sort_rand);
    RUN_TEST(test_sort_spinlock_abuse);
    RUN_TEST(test_sort_stale_thread_mem);
    RUN_TEST(test_sort_u_FMR);
    RUN_TEST(test_sort_unique_segv);
    RUN_TEST(test_sort_version);
    RUN_TEST(test_sort_sort_files0_from);
    RUN_TEST(test_sort_sort_merge);
    RUN_TEST(test_sort_sort_month);
    RUN_TEST(test_sort_sort);
}
DEFINE_TEST_MAIN(sort_suite)
