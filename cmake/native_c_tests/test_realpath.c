#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>



TEST test_realpath_basic(void) {
    FILE *f = fopen("test_realpath.txt", "w");
    fprintf(f, "data\n");
    if (f) fclose(f);
    char out[512];
    ASSERT_EQ(0, exec_capture("realpath test_realpath.txt", out, sizeof(out)));
    ASSERT(strstr(out, "test_realpath.txt") != NULL);
    remove("test_realpath.txt");
    PASS();
}

TEST test_realpath_advanced(void) {
    char out[1024];
    
    // Determine if // is distinct from /
    char double_slash[8] = "//";
#ifndef _WIN32
    struct stat st1, st2;
    if (stat("/", &st1) == 0 && stat("//", &st2) == 0) {
        if (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino) {
            strcpy(double_slash, "/");
        }
    }
#else
    strcpy(double_slash, "/"); // Windows simplify it
#endif

    mkdir("dir1", 0755);
    mkdir("dir1/dir2", 0755);
    
#ifndef _WIN32
    symlink("dir1/dir2", "ldir2");
#endif
    
    FILE *f = fopen("dir1/f", "w");
    if(f) if (f) fclose(f);
    f = fopen("dir1/dir2/f", "w");
    if(f) if (f) fclose(f);

#ifndef _WIN32
    symlink("/", "one");
    symlink("//", "two");
    symlink("///", "three");
#endif

    // Basic operation
    ASSERT_EQ(0, exec_capture("realpath -Pqz .", NULL, 0));
    
    // Operand is required
    { int _r = exec_capture("realpath", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("realpath --relative-base . --relative-to .", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("realpath --relative-base .", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    // -e --relative-* require directories
    { int _r = exec_capture("realpath -e --relative-to=dir1/f --relative-base=. .", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("realpath -e --relative-to=dir1/ --relative-base=. .", NULL, 0));
    
    // Check that using -E after -e uses -E
    ASSERT_EQ(0, exec_capture("realpath -e -E --relative-to=dir1/f --relative-base=. .", NULL, 0));

    // NUL params are unconditionally rejected by canonicalize_filename_mode
    { int _r = exec_capture("realpath -m ''", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("realpath --relative-base= --relative-to=. .", NULL, 0); ASSERT_EQ(1, WEXITSTATUS(_r)); }

#ifndef _WIN32
    // symlink resolution
    char cwd[1024];
    ASSERT_EQ(0, exec_capture("realpath .", cwd, sizeof(cwd)));
    trim_newline(cwd);
    
    char expected[1024];
    snprintf(expected, sizeof(expected), "%s/dir1\n", cwd);
    ASSERT_EQ(0, exec_capture("realpath ldir2/..", out, sizeof(out)));
    ASSERT_STR_EQ(expected, out);

    snprintf(expected, sizeof(expected), "%s\n", cwd);
    ASSERT_EQ(0, exec_capture("realpath -L ldir2/..", out, sizeof(out)));
    ASSERT_STR_EQ(expected, out);

    snprintf(expected, sizeof(expected), "%s/ldir2\n", cwd);
    ASSERT_EQ(0, exec_capture("realpath -s ldir2", out, sizeof(out)));
    ASSERT_STR_EQ(expected, out);
#endif

    // relative string handling
    ASSERT_EQ(0, exec_capture("realpath -m --relative-to=prefix prefixed/1", out, sizeof(out)));
    ASSERT_STR_EQ("../prefixed/1\n", out);

    ASSERT_EQ(0, exec_capture("realpath -m --relative-to=prefixed prefix/1", out, sizeof(out)));
    ASSERT_STR_EQ("../prefix/1\n", out);

    ASSERT_EQ(0, exec_capture("realpath -m --relative-to=prefixed prefixed/1", out, sizeof(out)));
    ASSERT_STR_EQ("1\n", out);

#ifndef _WIN32
    // Ensure no redundant trailing '/'
    ASSERT_EQ(0, exec_capture("realpath -sm --relative-to=/usr /", out, sizeof(out)));
    ASSERT_STR_EQ("..\n", out);

    // Ensure no redundant leading '../'
    ASSERT_EQ(0, exec_capture("realpath -sm --relative-to=/ /usr", out, sizeof(out)));
    ASSERT_STR_EQ("usr\n", out);

    // Ensure --relative-base works
    ASSERT_EQ(0, exec_capture("realpath -sm --relative-base=/usr --relative-to=/usr /tmp /usr", out, sizeof(out)));
    ASSERT_STR_EQ("/tmp\n.\n", out);

    ASSERT_EQ(0, exec_capture("realpath -sm --relative-base=/ --relative-to=/ / /usr", out, sizeof(out)));
    ASSERT_STR_EQ(".\nusr\n", out);

    // --relative-to defaults to the value of --relative-base
    ASSERT_EQ(0, exec_capture("realpath -sm --relative-base=/usr /tmp /usr", out, sizeof(out)));
    ASSERT_STR_EQ("/tmp\n.\n", out);

    ASSERT_EQ(0, exec_capture("realpath -sm --relative-base=/ / /usr", out, sizeof(out)));
    ASSERT_STR_EQ(".\nusr\n", out);

    ASSERT_EQ(0, exec_capture("realpath -sm --relative-base=/usr/local --relative-to=/usr /usr /usr/local", out, sizeof(out)));
    ASSERT_STR_EQ("/usr\n/usr/local\n", out);

    // Ensure // is handled correctly
    char expected_double[1024];
    snprintf(expected_double, sizeof(expected_double), "/\n%s\n/\n", double_slash);
    ASSERT_EQ(0, exec_capture("realpath / // ///", out, sizeof(out)));
    ASSERT_STR_EQ(expected_double, out);

    ASSERT_EQ(0, exec_capture("realpath one two three", out, sizeof(out)));
    ASSERT_STR_EQ(expected_double, out);

    ASSERT_EQ(0, exec_capture("realpath -sm --relative-to=/ / // /dev //dev", out, sizeof(out)));
    if (strcmp(double_slash, "//") == 0) {
        ASSERT_STR_EQ(".\n//\ndev\n//dev\n", out);
    } else {
        ASSERT_STR_EQ(".\n.\ndev\ndev\n", out);
    }

    ASSERT_EQ(0, exec_capture("realpath -sm --relative-to=// / // /dev //dev", out, sizeof(out)));
    if (strcmp(double_slash, "//") == 0) {
        ASSERT_STR_EQ("/\n.\n/dev\ndev\n", out);
    } else {
        ASSERT_STR_EQ(".\n.\ndev\ndev\n", out);
    }

    ASSERT_EQ(0, exec_capture("realpath --relative-base=/ --relative-to=// / //", out, sizeof(out)));
    if (strcmp(double_slash, "//") == 0) {
        ASSERT_STR_EQ("/\n//\n", out);
    } else {
        ASSERT_STR_EQ(".\n.\n", out);
    }
#endif

    unlink("one");
    unlink("two");
    unlink("three");
    unlink("ldir2");
    unlink("dir1/dir2/f");
    unlink("dir1/f");
    rmdir("dir1/dir2");
    rmdir("dir1");

    PASS();
}





SUITE(realpath_suite) {
    RUN_TEST(test_realpath_basic);
    RUN_TEST(test_realpath_advanced);
}
DEFINE_TEST_MAIN(realpath_suite)
