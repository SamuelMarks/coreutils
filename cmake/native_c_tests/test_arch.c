#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>





TEST test_arch_basic(void) {
    char out_arch[512];
    ASSERT_EQ(0, exec_capture("arch", out_arch, sizeof(out_arch)));
    trim_newline(out_arch);
    ASSERT(strlen(out_arch) > 0);
    
    char out_uname[512];
    ASSERT_EQ(0, exec_capture("uname -m", out_uname, sizeof(out_uname)));
    trim_newline(out_uname);
    
    ASSERT_STR_EQ(out_uname, out_arch);

    char out_arch_dash[512];
    ASSERT_EQ(0, exec_capture("arch --", out_arch_dash, sizeof(out_arch_dash)));
    trim_newline(out_arch_dash);
    ASSERT_STR_EQ(out_uname, out_arch_dash);

    PASS();
}

TEST test_arch_sh(void) {
    char out1[1024];
    char out2[1024];
#ifndef _WIN32
    ASSERT_EQ(0, exec_capture("arch", out1, sizeof(out1)));
    ASSERT_EQ(0, exec_capture("uname -m", out2, sizeof(out2)));
    ASSERT_STR_EQ(out2, out1);
    
    char out3[1024];
    ASSERT_EQ(0, exec_capture("arch --", out3, sizeof(out3)));
    ASSERT_STR_EQ(out2, out3);
#endif
    PASS();
}



SUITE(arch_suite) {
    RUN_TEST(test_arch_basic);
    RUN_TEST(test_arch_sh);
}
DEFINE_TEST_MAIN(arch_suite)
