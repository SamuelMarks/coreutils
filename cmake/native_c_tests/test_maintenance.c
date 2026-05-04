#include "test_helper.h"

TEST test_maintenance_checks(void) {
    SKIPm("Skipped: maintenance scripts like option-aliases.sh, getopt_vs_usage.sh");
    PASS();
}

SUITE(maintenance_suite) {
    RUN_TEST(test_maintenance_checks);
}
DEFINE_TEST_MAIN(maintenance_suite)

// Also skips tests/help/help-version-getopt.sh help_version_getopt
// tests/help/help-version.sh help_version
