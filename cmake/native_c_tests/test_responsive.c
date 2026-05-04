#include "test_helper.h"

TEST test_responsive_basic(void) {
    SKIPm("Skipped: highly timing and pipe dependent test");
    PASS();
}

SUITE(responsive_suite) {
    RUN_TEST(test_responsive_basic);
}
DEFINE_TEST_MAIN(responsive_suite)
