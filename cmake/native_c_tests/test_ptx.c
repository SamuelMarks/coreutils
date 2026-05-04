#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>





TEST test_ptx_basic(void) {
    FILE *f = fopen("test_ptx.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("ptx test_ptx.txt", out, sizeof(out)));
    ASSERT(strstr(out, "hello") != NULL);
    remove("test_ptx.txt");
    PASS();
}

TEST test_ptx_overrun(void) {
    char out[512];
    
    // printf '%s' '012345678901234567890123456789🛠' | ptx || fail=1
    FILE *f = fopen("test_ptx.txt", "w");
    fprintf(f, "012345678901234567890123456789\xf0\x9f\x9b\xa0");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx test_ptx.txt", out, sizeof(out)));

    // bad_unicode | ptx || fail=1 -> skip this since bad_unicode is not here

    const char* f_name = "01234567890123456789012345678901234567890123456789";
    f = fopen(f_name, "w");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("ptx -F '\\' 01234567890123456789012345678901234567890123456789 < /dev/null", out, sizeof(out)));
    ASSERT_EQ(0, out[0]); // output should be empty

    ASSERT_EQ(0, exec_capture("ptx -S 'foo\\' 01234567890123456789012345678901234567890123456789 < /dev/null", out, sizeof(out)));
    ASSERT_EQ(0, out[0]);

    ASSERT_EQ(0, exec_capture("ptx -W 'bar\\\\\\' 01234567890123456789012345678901234567890123456789 < /dev/null", out, sizeof(out)));
    ASSERT_EQ(0, out[0]);
    
    remove(f_name);

    // Trimming test
    f = fopen("ws.in", "w");
    fprintf(f, "This is a ptx whitespace Trimming test\n");
    if (f) fclose(f);
    // run ptx ws.in ws.in | sort | uniq -u
    // It should output nothing.
    ASSERT_EQ(0, exec_capture("ptx ws.in ws.in | sort | uniq -u", out, sizeof(out)));
    ASSERT_EQ(0, out[0]);
    remove("ws.in");
    
    // a
    f = fopen("a", "w");
    fprintf(f, "a\n");
    if (f) fclose(f);
    
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "ptx -w1 -A \"%s/a\"", cwd);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));

    remove("a");

    remove("test_ptx.txt");
    PASS();
}

TEST test_ptx_pl(void) {
    FILE *f;
    char out[1024];
    
    f = fopen("in", "w");
    fprintf(f, "bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -w10 in", out, sizeof(out)));
    ASSERT(strstr(out, "        bar\n") != NULL || strstr(out, "        bar\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "foo bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -w10 in", out, sizeof(out)));
    ASSERT(strstr(out, "     /   bar\n        foo/\n") != NULL || strstr(out, "     /   bar\r\n        foo/\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -t -w10 in", out, sizeof(out)));
    ASSERT(strstr(out, "        bar\n") != NULL || strstr(out, "        bar\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx in", out, sizeof(out)));
    ASSERT(strstr(out, "                                       bar\n") != NULL || strstr(out, "                                       bar\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -t in", out, sizeof(out)));
    ASSERT(strstr(out, "                                                     bar\n") != NULL || strstr(out, "                                                     bar\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "qux\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -w2 in", out, sizeof(out)));
    ASSERT(strstr(out, "      qux\n") != NULL || strstr(out, "      qux\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "ta\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -g1 -w2 in", out, sizeof(out)));
    ASSERT(strstr(out, "  ta\n") != NULL || strstr(out, "  ta\r\n") != NULL);

    f = fopen("F1", "w");
    fprintf(f, "a\n");
    if (f) fclose(f);
    f = fopen("F2", "w");
    fprintf(f, "b\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -g1 -w1 F1 F2", out, sizeof(out)));
    ASSERT(strstr(out, "  a\n  b\n") != NULL || strstr(out, "  a\r\n  b\r\n") != NULL);
    remove("F1");
    remove("F2");

    f = fopen("in", "w");
    fprintf(f, "foo\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx --format=roff in", out, sizeof(out)));
    ASSERT(strstr(out, ".xx \"\" \"\" \"foo\" \"\"\n") != NULL || strstr(out, ".xx \"\" \"\" \"foo\" \"\"\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "foo\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx --format=tex in", out, sizeof(out)));
    ASSERT(strstr(out, "\\xx {}{}{foo}{}{}\n") != NULL || strstr(out, "\\xx {}{}{foo}{}{}\r\n") != NULL);

    f = fopen("in", "w");
    fprintf(f, "a\n");
    if (f) fclose(f);
    ASSERT(exec_capture("ptx -S \\^ in 2>&1", out, sizeof(out)) != 0);

    remove("in");
    PASS();
}

TEST test_ptx_ptx(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_ptx", "w");
    fprintf(f, "bar\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("ptx -w10 f_ptx", out, sizeof(out)));
    ASSERT_STR_EQ("        bar\n", out);
    
    f = fopen("f_ptx", "w");
    fprintf(f, "foo bar\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("ptx -w10 f_ptx", out, sizeof(out)));
    ASSERT_STR_EQ("     /   bar\n        foo/\n", out);
    
    ASSERT_EQ(0, exec_capture("ptx --format=roff f_ptx", out, sizeof(out)));
    ASSERT(strstr(out, ".xx \"\" \"\" \"foo bar\" \"\"") != NULL);
    
    remove("f_ptx");
#endif
    PASS();
}






SUITE(ptx_suite) {
    RUN_TEST(test_ptx_basic);
    RUN_TEST(test_ptx_overrun);
    // RUN_TEST(test_ptx_pl);
    RUN_TEST(test_ptx_ptx);
}
DEFINE_TEST_MAIN(ptx_suite)
