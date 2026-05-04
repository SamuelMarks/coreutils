#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>




TEST test_pathchk_basic(void) {
    ASSERT_EQ(0, exec_capture("pathchk valid_path/test.txt", NULL, 0));
    ASSERT_EQ(0, exec_capture("pathchk --version", NULL, 0));
    PASS();
}

TEST test_pathchk_fail(void) {
    if (getuid() == 0) {
        PASS(); // skip if root
    }

    FILE *f = fopen("file", "w");
    if (f) if (f) fclose(f);

    { int _r = exec_capture("pathchk file/x 2> err", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    // This should exit nonzero. Through 5.3.0 it exited with status zero.
    { int _r = exec_capture("pathchk -p '' 2> err", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    // This tests the new -P option.
    { int _r = exec_capture("pathchk -P '' 2> err", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("pathchk -P -- - 2> err", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("pathchk -p -P x/- 2> err", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    remove("file");
    remove("err");

    PASS();
}




SUITE(pathchk_suite) {
    RUN_TEST(test_pathchk_basic);
    RUN_TEST(test_pathchk_fail);
}
DEFINE_TEST_MAIN(pathchk_suite)
