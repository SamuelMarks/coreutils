#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_base64_basic(void) {
    FILE *f = fopen("test_b64.txt", "w");
    fprintf(f, "hello world");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("base64 test_b64.txt", out, sizeof(out)));
    /* Check for the base64 hash of "hello world" */
    ASSERT(strstr(out, "aGVsbG8gd29ybGQ=") != NULL);
    remove("test_b64.txt");
    PASS();
}



SUITE(base64_suite) {
    RUN_TEST(test_base64_basic);
}
DEFINE_TEST_MAIN(base64_suite)
