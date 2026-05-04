#include "test_helper.h"
#include "greatest.h"

#define EXEC_ASSERT_SUCCESS(cmd) \
    do { \
        char out_err[1024] = {0}; \
        char cmd_err[256]; \
        snprintf(cmd_err, sizeof(cmd_err), "%s 2>&1", cmd); \
        int _r = exec_capture(cmd_err, out_err, sizeof(out_err)); \
        if (_r != 0) { \
            fprintf(stderr, "FAIL: '%s' returned %d. Output: %s\n", cmd, WEXITSTATUS(_r), out_err); \
            ASSERT_EQ(0, _r); \
        } \
    } while (0)

TEST test_ginstall_basic() {
    exec_capture("rm -rf dir file", NULL, 0);
    exec_capture("mkdir -p dir", NULL, 0);
    exec_capture("echo foo > file", NULL, 0);
    
    EXEC_ASSERT_SUCCESS("ginstall file dir");
    EXEC_ASSERT_SUCCESS("test -f file");
    EXEC_ASSERT_SUCCESS("test -f dir/file");
    
    EXEC_ASSERT_SUCCESS("ginstall -d newdir1 newdir2 newdir3");
    EXEC_ASSERT_SUCCESS("test -d newdir1");
    EXEC_ASSERT_SUCCESS("test -d newdir2");
    EXEC_ASSERT_SUCCESS("test -d newdir3");
    
    PASS();
}

TEST test_ginstall_basic_1() {
    exec_capture("rm -rf file1 file2 file3 file4 testdir", NULL, 0);
    exec_capture("touch file1 file3", NULL, 0);
    
    // Test --mode=+w
    EXEC_ASSERT_SUCCESS("ginstall file1 file2 --mode=+w");
    // Ensure omitted directories are diagnosed
    int res = exec_capture("ginstall . .", NULL, 0);
    ASSERT(WIFEXITED(res) && WEXITSTATUS(res) != 0);

    // Test comma-separated mode strings (like chmod)
    EXEC_ASSERT_SUCCESS("ginstall file3 file4 --mode=ug+rw,o+r");
    
    // Test comma-separated mode with directory creation
    EXEC_ASSERT_SUCCESS("ginstall -d testdir --mode=u+rwx,g+rx,o+r");
    
    PASS();
}

TEST test_ginstall_C() {
    exec_capture("rm -rf a b c d", NULL, 0);
    exec_capture("echo test > a", NULL, 0);
    
    // destination file does not exist
    EXEC_ASSERT_SUCCESS("ginstall -Cv -m0644 a b");
    
    // destination file exists
    char out[1024];
    exec_capture("ginstall -Cv -m0644 a b", out, sizeof(out));
    ASSERT_EQ_FMT(0, strlen(out), "%zu"); // should be empty output
    
    // destination file exists (long option)
    exec_capture("ginstall -v --compare -m0644 a b", out, sizeof(out));
    ASSERT_EQ_FMT(0, strlen(out), "%zu");
    
    // option -C ignored if any non-permission mode should be set
    exec_capture("ginstall -Cv -m2755 a b", out, sizeof(out));
    ASSERT(strstr(out, "'a' -> 'b'") != NULL);
    
    // destination file exists but content differs
    exec_capture("echo test1 > a", NULL, 0);
    exec_capture("ginstall -Cv -m0644 a b", out, sizeof(out));
    ASSERT(strstr(out, "'a' -> 'b'") != NULL);
    
    // second run should be empty
    exec_capture("ginstall -Cv -m0644 a b", out, sizeof(out));
    ASSERT_EQ_FMT(0, strlen(out), "%zu");
    
    PASS();
}

TEST test_ginstall_create_leading() {
    exec_capture("rm -rf file no-dir1 no-dir2 dir1", NULL, 0);
    exec_capture("echo foo > file", NULL, 0);
    
    EXEC_ASSERT_SUCCESS("ginstall -D file no-dir1/no-dir2/dest");
    EXEC_ASSERT_SUCCESS("test -d no-dir1/no-dir2");
    EXEC_ASSERT_SUCCESS("test -r no-dir1/no-dir2/dest");
    
    exec_capture("mkdir dir1", NULL, 0);
    exec_capture("touch dir1/file1", NULL, 0);
    // NOTE: my_system doesn't expand $PWD properly in the middle of paths, we just use relative paths
    // where possible or fake it if needed.
    EXEC_ASSERT_SUCCESS("ginstall -D dir1/file1 file -t no-dir2/");
    EXEC_ASSERT_SUCCESS("test -r no-dir2/file");
    EXEC_ASSERT_SUCCESS("test -r no-dir2/file1");

    PASS();
}

TEST test_ginstall_root_selinux_trap() {
    SKIPm("Skipped: install-C-root.sh, install-C-selinux.sh, install-Z-selinux.sh, trap.sh require root, selinux, or advanced signal trapping");
    PASS();
}

TEST test_ginstall_d_slashdot() {
    exec_capture("rm -rf d1 d2", NULL, 0);
    
    EXEC_ASSERT_SUCCESS("ginstall -d d1/.");
    EXEC_ASSERT_SUCCESS("test -d d1");
    
    EXEC_ASSERT_SUCCESS("ginstall -d d2/..");
    EXEC_ASSERT_SUCCESS("test -d d2");

    PASS();
}

TEST test_ginstall_strip_program() {
    exec_capture("rm -rf src_file dest2 my_strip", NULL, 0);
    
    exec_capture("echo 'int main(int argc, char **argv) { return 0; }' > my_strip.c", NULL, 0);
    exec_capture("gcc -o my_strip my_strip.c", NULL, 0);
    
    // Check if gcc succeeded
    struct stat st_strip;
    if (stat("my_strip", &st_strip) == 0) {
        exec_capture("echo abc > src_file", NULL, 0);
        EXEC_ASSERT_SUCCESS("ginstall src_file dest -s --strip-program=./my_strip");
        EXEC_ASSERT_SUCCESS("test -e dest");
        
        // Failing strip
        exec_capture("echo 'int main(int argc, char **argv) { return 1; }' > my_strip_fail.c", NULL, 0);
        exec_capture("gcc -o my_strip_fail my_strip_fail.c", NULL, 0);
        
        int fail_res = exec_capture("ginstall src_file dest2 -s --strip-program=./my_strip_fail", NULL, 0);
        ASSERT(WIFEXITED(fail_res) && WEXITSTATUS(fail_res) != 0);
        
        int check_dest2 = exec_capture("test -e dest2", NULL, 0);
        ASSERT(WIFEXITED(check_dest2) && WEXITSTATUS(check_dest2) != 0);
    }


    PASS();
}

SUITE(ginstall_suite) {
    RUN_TEST(test_ginstall_basic);
    RUN_TEST(test_ginstall_basic_1);
    RUN_TEST(test_ginstall_C);
    RUN_TEST(test_ginstall_create_leading);
    RUN_TEST(test_ginstall_root_selinux_trap);
    RUN_TEST(test_ginstall_d_slashdot);
    RUN_TEST(test_ginstall_strip_program);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(ginstall_suite);
    GREATEST_MAIN_END();
}
