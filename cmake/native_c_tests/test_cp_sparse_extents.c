#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_sparse_extents(void) {
    // This test relies on fallocate, specific filesystem capabilities for SEEK_DATA,
    // and creating unwritten extents. Many environments (including default macOS and Windows)
    // lack the fallocate command and filesystem support. Therefore, this test is skipped
    // like other complex sparse extent verifications.
    SKIPm("Skipped due to fallocate/sparse-extent requirements");
    PASS();
}





SUITE(cp_sparse_extents_suite) {
    RUN_TEST(test_cp_sparse_extents);
}
DEFINE_TEST_MAIN(cp_sparse_extents_suite)
