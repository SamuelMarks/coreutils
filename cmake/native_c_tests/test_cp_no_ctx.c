#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_no_ctx(void) {
    // This test relies on LD_PRELOAD to mock SELinux's getfilecon to return ENODATA.
    // LD_PRELOAD testing is skipped in the native C test suite.
    SKIPm("Skipped due to LD_PRELOAD requirements for SELinux mocking");
    PASS();
}





SUITE(cp_no_ctx_suite) {
    RUN_TEST(test_cp_no_ctx);
}
DEFINE_TEST_MAIN(cp_no_ctx_suite)
