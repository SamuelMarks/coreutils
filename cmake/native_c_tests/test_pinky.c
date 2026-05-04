#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_pinky_basic(void) {
    ASSERT_EQ(0, exec_capture("pinky", NULL, 0));
    PASS();
}



SUITE(pinky_suite) {
    RUN_TEST(test_pinky_basic);
}
DEFINE_TEST_MAIN(pinky_suite)
