#include "test_helper.h"

TEST test_tty_eof_basic(void) {
    SKIPm("Skipped: requires pty abstraction, same as Expect.pm in perl");
    PASS();
}

SUITE(tty_eof_suite) {
    RUN_TEST(test_tty_eof_basic);
}
DEFINE_TEST_MAIN(tty_eof_suite)
