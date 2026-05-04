#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sha384sum_basic(void) {
    FILE *f = fopen("test_sha384.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sha384sum test_sha384.txt", out, sizeof(out)));
    ASSERT(strstr(out, "test_sha384.txt") != NULL);
    remove("test_sha384.txt");
    PASS();
}



SUITE(sha384sum_suite) {
    RUN_TEST(test_sha384sum_basic);
}
DEFINE_TEST_MAIN(sha384sum_suite)
