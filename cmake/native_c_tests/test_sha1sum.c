#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sha1sum_basic(void) {
    FILE *f = fopen("test_sha1.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sha1sum test_sha1.txt", out, sizeof(out)));
    ASSERT(strstr(out, "22596363b3de40b06f981fb85d82312e8c0ed511") != NULL);
    remove("test_sha1.txt");
    PASS();
}



SUITE(sha1sum_suite) {
    RUN_TEST(test_sha1sum_basic);
}
DEFINE_TEST_MAIN(sha1sum_suite)
