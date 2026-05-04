#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sum_basic(void) {
    FILE *f = fopen("test_sum.txt", "w");
    fprintf(f, "hello");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sum test_sum.txt", out, sizeof(out)));
    ASSERT(strstr(out, "test_sum.txt") != NULL);
    remove("test_sum.txt");
    PASS();
}



SUITE(sum_suite) {
    RUN_TEST(test_sum_basic);
}
DEFINE_TEST_MAIN(sum_suite)
