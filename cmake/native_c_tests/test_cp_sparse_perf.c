#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_sparse_perf(void) {
    // This test relies on creating sparse files across partitions to test performance,
    // explicitly turning off `reflink`, verifying SEEK_DATA, and generating huge files
    // in order to check if `cp` scales appropriately.
    // Testing cross-partition constraints and creating 1 TiB files in C unit testing
    // on cross-platform setups is problematic.
    SKIPm("Skipped due to cross-partition and performance scale requirements");
    PASS();
}





SUITE(cp_sparse_perf_suite) {
    RUN_TEST(test_cp_sparse_perf);
}
DEFINE_TEST_MAIN(cp_sparse_perf_suite)
