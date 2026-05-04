#include "test_helper.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>




TEST test_runcon_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("runcon --version", out, sizeof(out)));
    ASSERT(strstr(out, "runcon") != NULL);
    PASS();
}

TEST test_runcon_no_reorder(void) {
    char out[1024];
    
    int ret1 = exec_capture("runcon -j true 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(125, WEXITSTATUS(ret1));
    
    exec_capture("runcon root:system_r:unconfined_t:s0 true -j 2>&1", out, sizeof(out));
    ASSERT(strstr(out, "invalid option -- 'j'") == NULL);
    
    PASS();
}

TEST test_runcon_compute(void) {
    char out[512];
    
    int ret2 = exec_capture("runcon -c false 2>/dev/null", out, sizeof(out));
    int ret = WEXITSTATUS(ret2);
    if (ret != 125) {
        ASSERT_EQ(1, ret);
    }
    PASS();
}




SUITE(runcon_suite) {
    RUN_TEST(test_runcon_basic);
    RUN_TEST(test_runcon_no_reorder);
    RUN_TEST(test_runcon_compute);
}
DEFINE_TEST_MAIN(runcon_suite)
