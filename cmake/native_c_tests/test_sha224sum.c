#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sha224sum_basic(void) {
    FILE *f = fopen("test_sha224.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sha224sum test_sha224.txt", out, sizeof(out)));
    ASSERT(strstr(out, "test_sha224.txt") != NULL);
    remove("test_sha224.txt");
    PASS();
}



SUITE(sha224sum_suite) {
    RUN_TEST(test_sha224sum_basic);
}
DEFINE_TEST_MAIN(sha224sum_suite)
