#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>




TEST test_misc_user_basic(void) {
#ifdef __linux__
    char out[1024];
    
    // Check if unshare -U is available
    if (exec_capture("unshare -U unshare --version", out, sizeof(out)) != 0) {
        PASS(); // skip
    }
    
    // Check if we are root
    if (getuid() == 0) {
        PASS(); // skip if root
    }

    if (exec_capture("cat /proc/sys/kernel/overflowuid", out, sizeof(out)) != 0) {
        PASS(); // skip
    }
    trim_newline(out);
    
    char overflow_uid[256];
    strncpy(overflow_uid, out, sizeof(overflow_uid));

    char id_cmd[256];
    snprintf(id_cmd, sizeof(id_cmd), "id -un %s", overflow_uid);
    char id_out[1024];
    if (exec_capture(id_cmd, id_out, sizeof(id_out)) != 0) {
        PASS(); // skip if id fails
    }
    trim_newline(id_out);

    char whoami_cmd[512];
    snprintf(whoami_cmd, sizeof(whoami_cmd), "unshare -U %s/whoami", bin_dir);
    char whoami_out[1024];
    if (exec_capture(whoami_cmd, whoami_out, sizeof(whoami_out)) == 0) {
        trim_newline(whoami_out);
        ASSERT_STR_EQ(id_out, whoami_out);
    }

    char logname_cmd[512];
    snprintf(logname_cmd, sizeof(logname_cmd), "unshare -U %s/logname </dev/null 2> err", bin_dir);
    { int _r = exec_capture(logname_cmd, out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    char err_buf[1024];
    FILE *f = fopen("err", "r");
    if (f) {
        size_t len = fread(err_buf, 1, sizeof(err_buf) - 1, f);
        err_buf[len] = '\0';
        if (f) fclose(f);
        trim_newline(err_buf);
        ASSERT_STR_EQ("logname: no login name", err_buf);
    }
    remove("err");

#endif
    PASS();
}





SUITE(misc_user_suite) {
    RUN_TEST(test_misc_user_basic);
}
DEFINE_TEST_MAIN(misc_user_suite)
