#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_dircolors_basic(void) {
    char out[2048];
    ASSERT_EQ(0, exec_capture("dircolors -b", out, sizeof(out)));
    ASSERT(strstr(out, "LS_COLORS") != NULL);
    PASS();
}

TEST test_dircolors_pl(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("k", "w");
    fprintf(f, "exec\n");
    if (f) fclose(f);
    
    ret = exec_capture("dircolors -b k 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    f = fopen("quote", "w");
    fprintf(f, "exec 'echo Hello;:'\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("dircolors -b quote", out, sizeof(out)));
    ASSERT(strstr(out, "echo Hello") != NULL);

    f = fopen("other", "w");
    fprintf(f, "owt 40;33\n");
    if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("dircolors -b other", out, sizeof(out)));
    ASSERT(strstr(out, "tw=40;33") != NULL);

    remove("k");
    remove("quote");
    remove("other");
    PASS();
}




SUITE(dircolors_suite) {
    RUN_TEST(test_dircolors_basic);
    RUN_TEST(test_dircolors_pl);
}
DEFINE_TEST_MAIN(dircolors_suite)
