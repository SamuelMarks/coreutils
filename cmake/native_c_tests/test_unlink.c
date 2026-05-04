#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>




TEST test_unlink_basic(void) {
    FILE *f = fopen("test_unlink.txt", "w");
    fprintf(f, "data\n");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("unlink test_unlink.txt", NULL, 0));
    
    struct stat st;
    ASSERT(stat("test_unlink.txt", &st) != 0);
    PASS();
}



SUITE(unlink_suite) {
    RUN_TEST(test_unlink_basic);
}
DEFINE_TEST_MAIN(unlink_suite)
