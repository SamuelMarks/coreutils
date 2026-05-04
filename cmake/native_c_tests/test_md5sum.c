#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_md5sum_basic(void) {
    FILE *f = fopen("test_md5.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("md5sum test_md5.txt", out, sizeof(out)));
    /* Check for the well-known md5 hash of "hello world\n" */
    ASSERT(strstr(out, "6f5902ac237024bdd0c176cb93063dc4") != NULL);
    remove("test_md5.txt");
    PASS();
}



SUITE(md5sum_suite) {
    RUN_TEST(test_md5sum_basic);
}
DEFINE_TEST_MAIN(md5sum_suite)
