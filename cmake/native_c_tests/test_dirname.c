#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_dirname_simple(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("dirname a", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ(".", out);
    PASS();
}
TEST test_dirname_dir(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("dirname a/b", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("a", out);
    PASS();
}
TEST test_dirname_trailing(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("dirname a/b/", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("a", out);
    PASS();
}
TEST test_dirname_root(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("dirname /", out, sizeof(out))); trim_newline(out); ASSERT_STR_EQ("/", out);
    PASS();
}
TEST test_dirname_pl(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("dirname -z d/f e/g", out, sizeof(out)));
    ASSERT(out[0] == 'd' && out[1] == '\0' && out[2] == 'e' && out[3] == '\0');
    PASS();
}




SUITE(dirname_suite) {
    RUN_TEST(test_dirname_simple);
    RUN_TEST(test_dirname_dir);
    RUN_TEST(test_dirname_trailing);
    RUN_TEST(test_dirname_root);
    RUN_TEST(test_dirname_pl);
}
DEFINE_TEST_MAIN(dirname_suite)
