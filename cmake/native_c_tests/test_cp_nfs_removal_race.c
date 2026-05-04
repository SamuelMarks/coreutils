#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_nfs_removal_race(void) {
    // This test relies on LD_PRELOAD to simulate an NFS race condition where a destination
    // file is reported as existing by stat, but is removed before open is called.
    // Testing LD_PRELOAD in cross-platform native tests is unreliable and 
    // unsupported on Windows/macOS. We skip this test.
    SKIPm("Skipped due to LD_PRELOAD requirements");
    PASS();
}





SUITE(cp_nfs_removal_race_suite) {
    RUN_TEST(test_cp_nfs_removal_race);
}
DEFINE_TEST_MAIN(cp_nfs_removal_race_suite)
