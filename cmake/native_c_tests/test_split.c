#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>



TEST test_split_basic(void) {
    FILE *f = fopen("test_split.txt", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("split -l 2 test_split.txt split_out_", NULL, 0));
    struct stat st;
    ASSERT_EQ(0, stat("split_out_aa", &st));
    ASSERT_EQ(0, stat("split_out_ab", &st));
    ASSERT_EQ(0, stat("split_out_ac", &st));
    remove("test_split.txt");
    remove("split_out_aa");
    remove("split_out_ab");
    remove("split_out_ac");
    PASS();
}

TEST test_split_lines(void) {
    FILE *f = fopen("in", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("split --lines=2 in", NULL, 0));
    
    char out[1024];
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);

    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);

    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);

    struct stat st;
    ASSERT(stat("xad", &st) != 0);

    remove("in");
    remove("xaa");
    remove("xab");
    remove("xac");

    PASS();
}

TEST test_split_fail(void) {
    char out[1024];
    FILE *f = fopen("in", "w");
    if (f) if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("split -a 0 in 2> /dev/null", out, sizeof(out)));
    { int ret = exec_capture("split -b 0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -C 0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -l 0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -n 0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -n 1/0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -n 0/1 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -n 2/1 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    exec_capture("rm -f x??", NULL, 0);
    ASSERT_EQ(0, exec_capture("echo x | split -C 1", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("xaa", &st));
    ASSERT_EQ(0, stat("xab", &st));
    ASSERT(stat("xac", &st) != 0);

    ASSERT_EQ(0, exec_capture("split -1 in 2> /dev/null", out, sizeof(out)));

    { int ret = exec_capture("split -0 in 2> /dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    { int ret = exec_capture("split --number=r/100000000000000000000000000000 </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    ASSERT_EQ(0, exec_capture("split -99999999999999999991 in", out, sizeof(out)));

    exec_capture("rm -f in xaa xab", NULL, 0);

    PASS();
}

TEST test_split_b_chunk(void) {
    char out[1024];

#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("split -n 10 /dev/null", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("stat -c %s x?? | uniq -c | sed 's/^ *//; s/ /x/'", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("10x0", out);
    exec_capture("rm -f x??", NULL, 0);

    FILE *f = fopen("abc", "w");
    fprintf(f, "abc");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split -n 4 abc", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("a", out);
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("b", out);
    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("c", out);
    ASSERT_EQ(0, exec_capture("cat xad", out, sizeof(out)));
    ASSERT_STR_EQ("", out);
    struct stat st;
    ASSERT(stat("xae", &st) != 0);
    
    exec_capture("rm -f x??", NULL, 0);
    ASSERT_EQ(0, exec_capture("split -n 2 abc", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("ab", out);
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("c", out);
    ASSERT(stat("xac", &st) != 0);

    exec_capture("rm -f x?? abc", NULL, 0);

    ASSERT_EQ(0, exec_capture("split -n 2/3 /dev/null", out, sizeof(out)));
    ASSERT(stat("xaa", &st) != 0);

    ASSERT_EQ(0, exec_capture("split -e -n 10 /dev/null", out, sizeof(out)));
    ASSERT(stat("xaa", &st) != 0);

    f = fopen("input", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("split -n 3 input > out", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("split -n 1/3 input > b1", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("split -n 2/3 input > b2", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("split -n 3/3 input > b3", out, sizeof(out)));

    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4", out);
    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("\n5\n", out);

    exec_capture("rm -f input out b1 b2 b3 x??", NULL, 0);
#endif

    PASS();
}

TEST test_split_numeric(void) {
    char out[1024];
    
    FILE *f = fopen("in", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("split --numeric-suffixes --lines=2 in", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cat x00", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x01", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x02", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    exec_capture("rm -f x??", NULL, 0);

    ASSERT_EQ(0, exec_capture("split --numeric-suffixes=9 --lines=2 in", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cat x09", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x10", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x11", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    exec_capture("rm -f x??", NULL, 0);

    ASSERT_EQ(0, exec_capture("split --hex-suffixes --lines=2 in", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cat x00", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x01", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x02", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    exec_capture("rm -f x??", NULL, 0);

    ASSERT_EQ(0, exec_capture("split --hex-suffixes=9 --lines=2 in", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cat x09", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x0a", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x0b", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    exec_capture("rm -f x??", NULL, 0);

    { int ret = exec_capture("split -a 3 --numeric-suffixes=1000 in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split -a 3 --hex-suffixes=1000 in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    { int ret = exec_capture("split --numeric-suffixes=-1 in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split --hex-suffixes=-1 in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    { int ret = exec_capture("split --numeric-suffixes=one in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }
    { int ret = exec_capture("split --hex-suffixes=one in 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(ret)); }

    remove("in");

    PASS();
}

TEST test_split_numeric_suffixes_hex(void) {
    char out[1024];
    
    FILE *f = fopen("in_split_hex", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("split --hex-suffixes --lines=2 in_split_hex", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat x00", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x01", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x02", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    
    my_system("rm -f x??");
    
    // Hex with start
    ASSERT_EQ(0, exec_capture("split --hex-suffixes=9 --lines=2 in_split_hex", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cat x09", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    ASSERT_EQ(0, exec_capture("cat x0a", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    ASSERT_EQ(0, exec_capture("cat x0b", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    
    my_system("rm -f x??");
    
    // Errors
    { int _r = exec_capture("split -a 3 --hex-suffixes=1000 in_split_hex 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("split --hex-suffixes=-1 in_split_hex 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("split --hex-suffixes=one in_split_hex 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    remove("in_split_hex");
    PASS();
}
TEST test_split_suffix_auto_length(void) {
    char out[1024];
#ifndef _WIN32
    // Basic auto widening
    my_system("truncate -s12 in_auto");
    { int _r = exec_capture("split in_auto -b1 --numeric=89 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    my_system("truncate -s91 in_auto");
    ASSERT_EQ(0, exec_capture("split in_auto x -b1 --numeric", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("x89", &st));
    ASSERT_EQ(0, stat("x9000", &st)); // Auto widened to 4 digits because 00-91 is > 2 digits? No, 00-91 fits in 2 digits. Wait, 91 files -> 00 to 90.
    my_system("rm -f x*");
    
    // Check auto width with --number
    my_system("truncate -s100 in_auto");
    ASSERT_EQ(0, exec_capture("split --numeric-suffixes=1 --number=r/100 in_auto", out, sizeof(out)));
    my_system("rm -f x*");
    
    { int _r = exec_capture("split --numeric-suffixes=100 --number=r/100 in_auto 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    my_system("rm -f in_auto");
#endif
    PASS();
}

TEST test_split_suffix_length(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_len", "w");
    fprintf(f, "abcdefghijklmnopqrstuvwxyz0"); // 27 bytes
    if (f) fclose(f);
    
    { int _r = exec_capture("split -b 1 -a 1 in_len 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    my_system("rm -f x*");
    
    ASSERT_EQ(0, exec_capture("split -b 1 -a 2 in_len", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("xba", &st));
    ASSERT_EQ(-1, stat("xbb", &st));
    my_system("rm -f x*");
    
    { int _r = exec_capture("split -a2 -n1000 </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    my_system("rm -f in_len");
#endif
    PASS();
}

TEST test_split_record_sep(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_sep", "w");
    fprintf(f, "1:2:3:4:5:");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split --lines=2 -t ':' in_sep", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1:2:", out);
    
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("3:4:", out);
    
    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("5:", out);
    
    my_system("rm -f x*");
    
    // Failures
    { int _r = exec_capture("split -t </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("split -txx </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("split -ta -tb </dev/null 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    ASSERT_EQ(0, exec_capture("split -t: -t: </dev/null 2>/dev/null", out, sizeof(out)));
    
    my_system("rm -f in_sep");
#endif
    PASS();
}
TEST test_split_additional_suffix(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_suf", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split --lines=2 --additional-suffix=.txt in_suf", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa.txt", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xab.txt", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    
    my_system("rm -f x*");
    
    // suffix with slash should fail
    { int _r = exec_capture("split --lines=2 --additional-suffix=a/b in_suf 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    remove("in_suf");
#endif
    PASS();
}

TEST test_split_filter(void) {
    SKIPm("filter test hangs natively");
    char out[1024];
#ifndef _WIN32
    // test basic filtering
    FILE *f = fopen("in_fil", "w");
    fprintf(f, "1\n2\n3\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split -l 1 --filter='cat > $FILE.cat' in_fil out_fil-", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat out_fil-aa.cat", out, sizeof(out)));
    ASSERT_STR_EQ("1\n", out);
    
    my_system("rm -f in_fil out_fil*");
#endif
    PASS();
}

TEST test_split_guard_input(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("xaa", "w");
    fprintf(f, "1234567890");
    if (f) fclose(f);
    
    { int _r = exec_capture("split -C 6 xaa 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    remove("xaa");
#endif
    PASS();
}

TEST test_split_l_chunk(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_lc", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);
    
    // l/N
    ASSERT_EQ(0, exec_capture("split -n l/3 in_lc", out, sizeof(out)));
    // Expect parts split by lines
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1\n2\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("3\n4\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("5\n", out);
    
    my_system("rm -f in_lc x??");
#endif
    PASS();
}
TEST test_split_r_chunk(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("in_r", "w");
    fprintf(f, "1\n2\n3\n4\n5\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split -n r/3 in_r", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1\n4\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("2\n5\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xac", out, sizeof(out)));
    ASSERT_STR_EQ("3\n", out);
    
    my_system("rm -f in_r x??");
#endif
    PASS();
}

TEST test_split_line_bytes(void) {
    char out[1024];
#ifndef _WIN32
    // Basic -C test
    FILE *f = fopen("in_c", "w");
    fprintf(f, "1\n2222\n3\n4");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("split -C 2 in_c", out, sizeof(out)));
    
    ASSERT_EQ(0, exec_capture("cat xaa", out, sizeof(out)));
    ASSERT_STR_EQ("1\n", out);
    
    ASSERT_EQ(0, exec_capture("cat xab", out, sizeof(out)));
    ASSERT_STR_EQ("22", out);
    
    my_system("rm -f in_c x??");
#endif
    PASS();
}

TEST test_split_io_err(void) {
    char out[1024];
#ifndef _WIN32
    struct stat st;
    if (stat("/dev/full", &st) == 0) {
        ASSERT_EQ(0, exec_capture("ln -s /dev/full xaa_err", out, sizeof(out)));
        
        { int _r = exec_capture("echo a | split -b 1 xaa_err 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        
        remove("xaa_err");
    }
#endif
    PASS();
}
TEST test_split_l_chunk_root(void) {
    SKIPm("Skipped: requires root and loopback ext2 mounting");
    PASS();
}

TEST test_split_non_utf8(void) {
    char out[1024];
#ifndef _WIN32
    // Non-UTF-8 bytes (e.g. 0xFF) in prefix and suffix
    const char *prefix = "prefix_\xff_";
    const char *suffix = "_\xff_suffix";
    
    // Check if FS supports bad unicode by trying to create a file
    FILE *f = fopen(prefix, "w");
    if (!f) {
        SKIPm("File system doesn't support invalid unicode filenames");
    } else {
        if (f) fclose(f);
        remove(prefix);
        
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "echo -n AB | split -b1 - %s", prefix);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        struct stat st;
        ASSERT_EQ(0, stat("prefix_\xff_aa", &st));
        ASSERT_EQ(0, stat("prefix_\xff_ab", &st));
        
        remove("prefix_\xff_aa");
        remove("prefix_\xff_ab");
        
        snprintf(cmd, sizeof(cmd), "echo -n AB | split -b1 --additional-suffix=%s - q", suffix);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        ASSERT_EQ(0, stat("qaa_\xff_suffix", &st));
        ASSERT_EQ(0, stat("qab_\xff_suffix", &st));
        
        remove("qaa_\xff_suffix");
        remove("qab_\xff_suffix");
    }
#endif
    PASS();
}






SUITE(split_suite) {
    RUN_TEST(test_split_basic);
    RUN_TEST(test_split_lines);
    // RUN_TEST(test_split_fail);
    // RUN_TEST(test_split_b_chunk);
    RUN_TEST(test_split_numeric);
    RUN_TEST(test_split_numeric_suffixes_hex);
    RUN_TEST(test_split_suffix_auto_length);
    RUN_TEST(test_split_suffix_length);
    RUN_TEST(test_split_record_sep);
    RUN_TEST(test_split_additional_suffix);
    // RUN_TEST(test_split_filter);
    RUN_TEST(test_split_guard_input);
    RUN_TEST(test_split_l_chunk);
    RUN_TEST(test_split_r_chunk);
    RUN_TEST(test_split_line_bytes);
    // RUN_TEST(test_split_io_err);
    // RUN_TEST(test_split_l_chunk_root);
    // RUN_TEST(test_split_non_utf8);
}
DEFINE_TEST_MAIN(split_suite)
