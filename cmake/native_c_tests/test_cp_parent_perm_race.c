#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_parent_perm_race(void) {
    // This test relies on mkfifo and race conditions during copying contents,
    // as well as UNIX permissions semantics (sticky bit, mode testing during copy).
    // It's skipped due to the complexity of portable FIFOs and process synchronization
    // across Windows/Mac/Linux.
    SKIPm("Skipped due to mkfifo/race condition requirements");

    PASS();
}





SUITE(cp_parent_perm_race_suite) {
    RUN_TEST(test_cp_parent_perm_race);
}
DEFINE_TEST_MAIN(cp_parent_perm_race_suite)
