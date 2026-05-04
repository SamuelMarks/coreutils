#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_who_basic(void) {
    ASSERT_EQ(0, exec_capture("who", NULL, 0));
    PASS();
}



SUITE(who_suite) {
    RUN_TEST(test_who_basic);
}
DEFINE_TEST_MAIN(who_suite)
