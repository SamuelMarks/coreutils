#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sha256sum_basic(void) {
    FILE *f = fopen("test_sha256.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sha256sum test_sha256.txt", out, sizeof(out)));
    ASSERT(strstr(out, "a948904f2f0f479b8f8197694b30184b0d2ed1c1cd2a1ec0fb85d299a192a447") != NULL);
    remove("test_sha256.txt");
    PASS();
}



SUITE(sha256sum_suite) {
    RUN_TEST(test_sha256sum_basic);
}
DEFINE_TEST_MAIN(sha256sum_suite)
