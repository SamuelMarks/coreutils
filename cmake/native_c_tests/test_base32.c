#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_base32_basic(void) {
    FILE *f = fopen("test_b32.txt", "w");
    fprintf(f, "hello world");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("base32 test_b32.txt", out, sizeof(out)));
    /* Check for the base32 hash of "hello world" */
    ASSERT(strstr(out, "NBSWY3DPEB3W64TMMQ======") != NULL);
    remove("test_b32.txt");
    PASS();
}



SUITE(base32_suite) {
    RUN_TEST(test_base32_basic);
}
DEFINE_TEST_MAIN(base32_suite)
