#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_sha512sum_basic(void) {
    FILE *f = fopen("test_sha512.txt", "w");
    fprintf(f, "hello world\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("sha512sum test_sha512.txt", out, sizeof(out)));
    /* Update check for the correct hash of "hello world\n" */
    ASSERT(strstr(out, "db3974a97f2407b7cae1ae637c0030687a11913274d578492558e39c16c017de84eacdc8c62fe34ee4e12b4b1428817f09b6a2760c3f8a664ceae94d2434a593") != NULL);
    remove("test_sha512.txt");
    PASS();
}



SUITE(sha512sum_suite) {
    RUN_TEST(test_sha512sum_basic);
}
DEFINE_TEST_MAIN(sha512sum_suite)
