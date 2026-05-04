#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>



TEST test_pwd_basic(void) {
    char out[1024];
    ASSERT_EQ(0, exec_capture("pwd", out, sizeof(out)));
    trim_newline(out);
    ASSERT(strlen(out) > 0);
    PASS();
}

TEST test_pwd_argument(void) {
    char out[1024];
    char err[1024];
    char cwd[1024];
    char cmd[1024];

    ASSERT(getcwd(cwd, sizeof(cwd)) != NULL);

    mkdir("a", 0755);
    mkdir("a/b", 0755);
    mkdir("a/b/c", 0755);
    ASSERT_EQ(0, chdir("a/b/c"));

    snprintf(cmd, sizeof(cmd), "%s/pwd a 2> ../../../err", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);

    ASSERT(strstr(out, "a/b/c") != NULL);

    char err_buf[1024];
    FILE *f = fopen("../../../err", "r");
    ASSERT(f != NULL);
    size_t len = fread(err_buf, 1, sizeof(err_buf) - 1, f);
    err_buf[len] = '\0';
    if (f) fclose(f);
    
    ASSERT(strstr(err_buf, "pwd: ignoring non-option arguments") != NULL);

    chdir("../../..");
    remove("err");
    remove("a/b/c");
    remove("a/b");
    remove("a");

    PASS();
}

TEST test_pwd_long(void) {
    char cwd[1024];
    ASSERT(getcwd(cwd, sizeof(cwd)) != NULL);

    const char *z = "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"; // 31 chars
    int n = 256;
    int i = 0;
    
    for (; i < n; i++) {
        if (mkdir(z, 0700) != 0) break;
        if (chdir(z) != 0) break;
    }

    if (i < n) {
        for (int j = 0; j < i; j++) {
            chdir("..");
            remove(z);
        }
        PASS();
    }

    char out[16384]; 
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "%s/pwd", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    
    ASSERT(strlen(out) > 7900);

    for (int j = 0; j < i; j++) {
        chdir("..");
        remove(z);
    }

    PASS();
}

TEST test_pwd_option(void) {
    char out[1024];
    char cmd[1024];
    char base[1024];

    ASSERT(getcwd(base, sizeof(base)) != NULL);

    mkdir("a", 0755);
    mkdir("a/b", 0755);

#ifndef _WIN32
    if (symlink("a/b", "c") != 0) {
        remove("a/b");
        remove("a");
        PASS(); // Skip if symlink fails
    }

    // Set up environments and test
    // To properly test logical paths we'd need to set PWD environment variable which exec_capture child inherits
    // using setenv
    chdir("c");

    // base/c is logical path, base/a/b is physical
    char expected_logical[1024];
    char expected_physical[1024];
    snprintf(expected_logical, sizeof(expected_logical), "%s/c", base);
    snprintf(expected_physical, sizeof(expected_physical), "%s/a/b", base);

    setenv("PWD", expected_logical, 1);

    snprintf(cmd, sizeof(cmd), "%s/pwd -L", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    ASSERT_EQ(0, strcmp(out, expected_logical));

    snprintf(cmd, sizeof(cmd), "%s/pwd --logical -P", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    ASSERT_EQ(0, strcmp(out, expected_physical));

    snprintf(cmd, sizeof(cmd), "%s/pwd --physical", bin_dir);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    trim_newline(out);
    ASSERT_EQ(0, strcmp(out, expected_physical));

    // Cleanup
    chdir(base);
    unlink("c");
#endif

    remove("a/b");
    remove("a");

    PASS();
}





SUITE(pwd_suite) {
    RUN_TEST(test_pwd_basic);
    RUN_TEST(test_pwd_argument);
    RUN_TEST(test_pwd_long);
    RUN_TEST(test_pwd_option);
}
DEFINE_TEST_MAIN(pwd_suite)
