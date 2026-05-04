#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_b2sum_basic(void) {
    FILE *f = fopen("test_b2.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("b2sum test_b2.txt", out, sizeof(out)));
    ASSERT(strstr(out, "test_b2.txt") != NULL);
    remove("test_b2.txt");
    PASS();
}



SUITE(b2sum_suite) {
    RUN_TEST(test_b2sum_basic);
}
DEFINE_TEST_MAIN(b2sum_suite)
