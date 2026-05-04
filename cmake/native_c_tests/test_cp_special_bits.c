#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



TEST test_cp_special_bits(void) {
    // This test requires root access and creating setuid/setgid files
    // and running commands as a non-root user via chroot.
    SKIPm("Skipped due to root/chroot requirements");
    PASS();
}





SUITE(cp_special_bits_suite) {
    RUN_TEST(test_cp_special_bits);
}
DEFINE_TEST_MAIN(cp_special_bits_suite)
