#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>




TEST test_link_basic(void) {
    FILE *f = fopen("test_link_in.txt", "w");
    fprintf(f, "data\n");
    if (f) fclose(f);
    remove("test_link_out.txt");
    int res = exec_capture("link test_link_in.txt test_link_out.txt", NULL, 0);
    if (res == 0) {
        struct stat st;
        ASSERT_EQ(0, stat("test_link_out.txt", &st));
        remove("test_link_out.txt");
    }
    remove("test_link_in.txt");
    PASS();
}



SUITE(link_suite) {
    RUN_TEST(test_link_basic);
}
DEFINE_TEST_MAIN(link_suite)
