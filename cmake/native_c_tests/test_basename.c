#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_basename_simple(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("basename a", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("a", out);
    PASS();
}
TEST test_basename_dir(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("basename a/b", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("b", out);
    PASS();
}
TEST test_basename_trailing(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("basename a/b/", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("b", out);
    PASS();
}
TEST test_basename_suffix(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("basename a/b.c .c", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("b", out);
    PASS();
}
TEST test_basename_root(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("basename /", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("/", out);
    PASS();
}
TEST test_basename_pl(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("basename -a d/f d/g", out, sizeof(out)));
    ASSERT(strstr(out, "f\ng\n") != NULL || strstr(out, "f\r\ng\r\n") != NULL);
    ASSERT_EQ(0, exec_capture("basename -a -s .c d/f.c d/g.c", out, sizeof(out)));
    ASSERT(strstr(out, "f\ng\n") != NULL || strstr(out, "f\r\ng\r\n") != NULL);
    ASSERT_EQ(0, exec_capture("basename -z d/f", out, sizeof(out)));
    ASSERT(out[0] == 'f' && out[1] == '\0');
    PASS();
}




SUITE(basename_suite) {
    RUN_TEST(test_basename_simple);
    RUN_TEST(test_basename_dir);
    RUN_TEST(test_basename_trailing);
    RUN_TEST(test_basename_suffix);
    RUN_TEST(test_basename_root);
    RUN_TEST(test_basename_pl);
}
DEFINE_TEST_MAIN(basename_suite)
