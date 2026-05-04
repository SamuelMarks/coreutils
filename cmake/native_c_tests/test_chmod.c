#include "test_helper.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>



TEST test_chmod_basic(void) {
    FILE *f = fopen("test_chmod.txt", "w");
    fprintf(f, "test");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("chmod 0644 test_chmod.txt", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("test_chmod.txt", &st));
    remove("test_chmod.txt");
    PASS();
}

TEST test_chmod_sh(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    
    // octal
    ASSERT_EQ(0, exec_capture("chmod 0644 f", out, sizeof(out)));
    // equals
    ASSERT_EQ(0, exec_capture("chmod a=rwx f", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("chmod u=rwx,g=rx,o= f", out, sizeof(out)));

    // c-option
    ASSERT_EQ(0, exec_capture("chmod -c 0644 f", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("chmod -c 0644 f", out, sizeof(out))); 
    
    // no-x
    ASSERT_EQ(0, exec_capture("chmod a-x f", out, sizeof(out)));
    
#ifndef _WIN32
    symlink("f", "sym");
    ASSERT_EQ(0, exec_capture("chmod -R 0644 sym 2>/dev/null", out, sizeof(out)));
    unlink("sym");
#endif
    
    ret = exec_capture("chmod 0644 no_such_file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    ret = exec_capture("chmod -f 0644 no_such_file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret)); 

    remove("f");
    PASS();
}

TEST test_chmod_usage(void) {
    char out[1024];
    int ret;
    
    // "--" ":"
    ret = exec_capture("chmod -- 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    // "-- --" ":"
    ret = exec_capture("chmod -- -- 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    // "-- -- -- f" ":" "-- f"
    FILE *f1 = fopen("--", "w"); fclose(f1);
    FILE *f2 = fopen("f", "w"); fclose(f2);
    ret = exec_capture("chmod -- -- -- f", out, sizeof(out));
    ASSERT_EQ(0, ret);
    
    // "-- -- -w f" ":" "-w f"
    FILE *f3 = fopen("-w", "w"); fclose(f3);
    ret = exec_capture("chmod -- -- -w f", out, sizeof(out));
    ASSERT_EQ(0, ret);

    // "-- -- f" ":" "f"
    ret = exec_capture("chmod -- -- f", out, sizeof(out));
    ASSERT_EQ(0, ret);

    // "-w -- f" ":" "f"
    ret = exec_capture("chmod -w -- f", out, sizeof(out));
    ASSERT_EQ(0, ret);

    // "-w -w f" ":" "f"
    ret = exec_capture("chmod -w -w f", out, sizeof(out));
    ASSERT_EQ(0, ret);

    // "u+gr f" ":" ""
    ret = exec_capture("chmod u+gr f 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    // "ug,+x f" ":" ""
    ret = exec_capture("chmod ug,+x f 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    remove("--");
    remove("-w");
    remove("f");
    PASS();
}

TEST test_chmod_equal_x(void) {
    char out[1024];
    FILE *f = fopen("f", "w"); if (f) fclose(f);

    umask(005);
    
    const char *modes[] = {"=x", "=xX", "=Xx", "=x,=X", "=X,=x"};
    for (int i = 0; i < 5; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "chmod a=r,%s f", modes[i]);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        struct stat st;
        ASSERT_EQ(0, stat("f", &st));
        
        // expected: ---x--x---
        // rwxrwxrwx = 0777. x for user and group = 0110
        // Wait, 'a=r,=x' applies to all. 'r' sets r--r--r--.
        // Then '=x' or '=X' overrides a=r to a=x.
        // But umask 005 means group and others get filtered? No, umask doesn't affect absolute modes unless the mode is missing specifying the who?
        // Wait, "a=" ignores umask. 
        // Let's check what `ls -l` outputs in shell script: `---x--x---`
        // 0110 in octal: user x, group x. wait, ---x--x--- is u=x, g=x, o=.
        // 0110 is correct. (S_IXUSR | S_IXGRP)
        mode_t expected = S_IXUSR | S_IXGRP; 
        ASSERT_EQ(expected, st.st_mode & 0777);
    }

    remove("f");
    PASS();
}

TEST test_chmod_ignore_symlink(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p dir_sym");
    FILE *f = fopen("dir_sym/f", "w"); if (f) fclose(f);
    symlink("f", "dir_sym/l");
    
    ASSERT_EQ(0, exec_capture("chmod u+w -R dir_sym 2> out", out, sizeof(out)));
    
    FILE *out_f = fopen("out", "r");
    if (out_f) {
        fseek(out_f, 0, SEEK_END);
        ASSERT_EQ(0, ftell(out_f)); // empty
        fclose(out_f);
    }
    
    unlink("dir_sym/l");
    remove("dir_sym/f");
    rmdir("dir_sym");
    remove("out");
#endif
    PASS();
}

TEST test_chmod_silent(void) {
    char out[1024];
    int ret;
    ret = exec_capture("chmod -f 0 no-such-file 2> out", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    FILE *out_f = fopen("out", "r");
    if (out_f) {
        fseek(out_f, 0, SEEK_END);
        ASSERT_EQ(0, ftell(out_f)); // empty
        fclose(out_f);
    }
    remove("out");
    PASS();
}

TEST test_chmod_partial_fail(void) {
    char out[1024];
    FILE *f = fopen("file", "w"); if (f) fclose(f);
    
    int ret = exec_capture("chmod 0 missing_file file 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    remove("file");
    PASS();
}

TEST test_chmod_inaccessible(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p d/e");
    ASSERT_EQ(0, exec_capture("chmod 0 d/e d", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("chmod u+rwx d d/e", out, sizeof(out)));
    rmdir("d/e");
    rmdir("d");
#endif
    PASS();
}

TEST test_chmod_thru_dangling(void) {
    char out[1024];
#ifndef _WIN32
    symlink("non-existent", "dangle");
    
    int ret = exec_capture("chmod 644 dangle 2> out", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    FILE *out_f = fopen("out", "r");
    char err_msg[256] = {0};
    if (out_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
        fclose(out_f);
    }
    trim_newline(err_msg);
    ASSERT_STR_EQ("chmod: cannot operate on dangling symlink 'dangle'", err_msg);
    
    unlink("dangle");
    remove("out");
#endif
    PASS();
}

TEST test_chmod_symlinks(void) {
    char out[1024];
#ifndef _WIN32
    my_system("mkdir -p a/b a/c");
    FILE *f1 = fopen("a/b/file", "w"); fclose(f1);
    FILE *f2 = fopen("a/c/file", "w"); fclose(f2);
    symlink("foo", "a/dangle");
    symlink("../b/file", "a/c/link");
    symlink("b", "a/dirlink");

    // reset
    ASSERT_EQ(0, exec_capture("chmod =777 a/b a/c a/b/file a/c/file", out, sizeof(out)));
    
    // -R (with default -H) does not deref traversed symlinks
    ASSERT_EQ(0, exec_capture("chmod 755 -R a/c", out, sizeof(out)));
    struct stat st;
    stat("a/c/file", &st); ASSERT_EQ(0755, st.st_mode & 0777);
    stat("a/b/file", &st); ASSERT_EQ(0777, st.st_mode & 0777); // unchanged

    // reset
    ASSERT_EQ(0, exec_capture("chmod =777 a/b a/c a/b/file a/c/file", out, sizeof(out)));

    // -LR a/c
    ASSERT_EQ(0, exec_capture("chmod 755 -LR a/c", out, sizeof(out)));
    stat("a/c/file", &st); ASSERT_EQ(0755, st.st_mode & 0777);
    stat("a/b/file", &st); ASSERT_EQ(0755, st.st_mode & 0777); // changed!

    // reset
    ASSERT_EQ(0, exec_capture("chmod =777 a/b a/c a/b/file a/c/file", out, sizeof(out)));

    // -RP a/c/
    ASSERT_EQ(0, exec_capture("chmod 755 -RP a/c/", out, sizeof(out)));
    stat("a/b/file", &st); ASSERT_EQ(0777, st.st_mode & 0777); // unchanged
    
    // reset
    ASSERT_EQ(0, exec_capture("chmod =777 a/b a/c a/b/file a/c/file", out, sizeof(out)));

    // --dereference a/c/link
    ASSERT_EQ(0, exec_capture("chmod 755 --dereference a/c/link", out, sizeof(out)));
    stat("a/b/file", &st); ASSERT_EQ(0755, st.st_mode & 0777); // changed

    // reset
    ASSERT_EQ(0, exec_capture("chmod =777 a/b a/c a/b/file a/c/file", out, sizeof(out)));

    // --no-dereference a/c/link
    int ret = exec_capture("chmod 755 --no-dereference a/c/link 2>err", out, sizeof(out));
    ASSERT_EQ(0, ret); // it succeeds (chmoding the link itself or ignoring)
    stat("a/b/file", &st); ASSERT_EQ(0777, st.st_mode & 0777); // unchanged

    // Dangling links
    ASSERT_EQ(0, exec_capture("chmod 755 --no-dereference -h a/dangle 2>err", out, sizeof(out)));
    { int _r = exec_capture("chmod 755 --deref a/dangle 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }

    my_system("rm -rf a err out");
#endif
    PASS();
}

TEST test_chmod_umask_x(void) {
    char out[1024];
    FILE *f = fopen("file_ux", "w"); if (f) fclose(f);
    chmod("file_ux", 0755);
    
    umask(0077);
    // '-x' takes away execution bits, but since umask is 077, it only takes away user exec bit?
    // wait, '-x' takes away from ALL according to umask? No, '-x' takes away from all that are not in umask.
    // umask 077 means group and other are masked. So '-x' only takes away 'u'.
    // If we take away 'u' exec bit, 'g' and 'o' might still be executable if they were set.
    // chmod -x is effectively chmod a-x, but restricted by umask. So only u-x is applied.
    // The resulting mode would be 0655. Since g and o are still executable, but we asked for '-x',
    // chmod detects this and reports an error? No, chmod just applies it, but maybe fails if the result is still executable and it was supposed to remove ALL executable bits?
    // Wait, the test says `returns_ 1 chmod -x file 2>/dev/null`.
    
    int ret = exec_capture("chmod -x file_ux 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    remove("file_ux");
    PASS();
}

TEST test_chmod_only_op(void) {
    char out[1024];
    
#ifndef _WIN32
    // test that chmod calls chmod(2) even when no bits change.
    // we use a file we don't own (e.g. / or a root owned file)
    struct stat st;
    if (stat("/", &st) == 0 && st.st_uid != geteuid()) {
        int ret;
        ret = exec_capture("chmod + / 2>err", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        
        FILE *err_f = fopen("err", "r");
        char err_msg[1024] = {0};
        if (err_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
            fclose(err_f);
        }
        ASSERT(strstr(err_msg, "changing permissions of") != NULL);
        
        ret = exec_capture("chmod - / 2>err", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));

        ret = exec_capture("chmod = / 2>err", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
    }
    remove("err");
#endif
    PASS();
}
TEST test_chmod_c_option(void) {
    char out[1024];
    
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    chmod("f", 0444);
    
    ASSERT_EQ(0, exec_capture("chmod u=rwx f", out, sizeof(out)));
    
    int ret = exec_capture("chmod -c g=rwx f 2>err", out, sizeof(out));
    ASSERT_EQ(0, ret);
    
    // Check if out contains "changed from"
    ASSERT(strstr(out, "changed from") != NULL);
    
    // Second time should be silent
    ret = exec_capture("chmod -c g=rwx f 2>err", out, sizeof(out));
    ASSERT_EQ(0, ret);
    ASSERT(strlen(out) == 0); // No output expected

#ifndef _WIN32
    my_system("mkdir -p a/b");
    chmod("a/b", 02755); // g+s
    ret = exec_capture("chmod -c -R g+w a 2>err", out, sizeof(out));
    ASSERT_EQ(0, ret);
    
    FILE *err_f = fopen("err", "r");
    if (err_f) {
        fseek(err_f, 0, SEEK_END);
        ASSERT_EQ(0, ftell(err_f)); // empty
        fclose(err_f);
    }
    my_system("rm -rf a");
#endif
    
    remove("err");
    remove("f");
    PASS();
}

TEST test_chmod_octal(void) {
    char out[1024];
    
    { int _r = exec_capture("chmod 0-anything . 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("chmod 7-anything . 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    { int _r = exec_capture("chmod 8 . 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    
    PASS();
}

TEST test_chmod_setgid(void) {
    char out[1024];
    
#ifndef _WIN32
    my_system("mkdir -p d_setgid");
    chmod("d_setgid", 0755);
    chmod("d_setgid", 0755 | S_ISGID);
    
    struct stat st;
    stat("d_setgid", &st);
    if ((st.st_mode & S_ISGID) == 0) {
        // Skip
        rmdir("d_setgid");
        PASS();
    }
    
    const char *modes[] = {"+", "-", "g-s", "00755", "000755", "=755", "-2000", "-7022", "755", "0755", "+2000", "-5022", "=7777,-5022"};
    for (int i = 0; i < 13; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "chmod %s d_setgid", modes[i]);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        stat("d_setgid", &st);
        int expect_setgid = 1;
        if (strcmp(modes[i], "g-s") == 0 || strncmp(modes[i], "00", 2) == 0 || strcmp(modes[i], "=755") == 0 || strcmp(modes[i], "-2000") == 0 || strcmp(modes[i], "-7022") == 0) {
            expect_setgid = 0;
        }
        
        if (expect_setgid) {
            ASSERT((st.st_mode & S_ISGID) != 0);
        } else {
            ASSERT((st.st_mode & S_ISGID) == 0);
        }
        
        // reset
        chmod("d_setgid", 0755 | S_ISGID);
    }
    
    rmdir("d_setgid");
#endif
    PASS();
}

TEST test_chmod_no_x(void) {
    char out[1024];
    
#ifndef _WIN32
    // Requires non-root
    if (geteuid() != 0) {
        my_system("mkdir -p d_nox/no-x/y");
        chmod("d_nox/no-x", 0600); // u=rw, no x
        
        int ret = exec_capture("chmod -R o=r d_nox 2>out", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        
        FILE *out_f = fopen("out", "r");
        char err_msg[1024] = {0};
        if (out_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
            fclose(out_f);
        }
        ASSERT(strstr(err_msg, "d_nox/no-x") != NULL);
        ASSERT(strstr(err_msg, "ermission denied") != NULL);
        
        chmod("d_nox/no-x", 0700);
        my_system("rm -rf d_nox");
    }
    remove("out");
#endif
    PASS();
}
TEST test_chmod_equals(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f = fopen("f_eq", "w"); if (f) fclose(f);
    
    // test copying permissions from u to g to o
    // "a=,u=rwx,g=u,u=" means:
    // clear all, u gets rwx, g copies from u (gets rwx), u gets cleared
    // final: ----rwx---
    ASSERT_EQ(0, exec_capture("chmod a=,u=rwx,g=u,u= f_eq", out, sizeof(out)));
    struct stat st;
    stat("f_eq", &st);
    ASSERT_EQ(0070, st.st_mode & 0777);
    
    // test dest=o, src=u
    ASSERT_EQ(0, exec_capture("chmod a=,u=rwx,o=u,u= f_eq", out, sizeof(out)));
    stat("f_eq", &st);
    ASSERT_EQ(0007, st.st_mode & 0777);
    
    // test dest=u, src=g
    ASSERT_EQ(0, exec_capture("chmod a=,g=rwx,u=g,g= f_eq", out, sizeof(out)));
    stat("f_eq", &st);
    ASSERT_EQ(0700, st.st_mode & 0777);
    
    // test =u with umask
    umask(0027);
    ASSERT_EQ(0, exec_capture("chmod a=,u=rwx,=u f_eq", out, sizeof(out)));
    stat("f_eq", &st);
    // =u means a=u but restricted by umask.
    // a gets rwx, but umask 027 strips w from group, and rwx from other
    // final: rwxr-x--- -> 0750
    ASSERT_EQ(0750, st.st_mode & 0777);
    
    remove("f_eq");
#endif
    PASS();
}


SUITE(chmod_suite) {
    RUN_TEST(test_chmod_basic);
    RUN_TEST(test_chmod_sh);
    RUN_TEST(test_chmod_usage);
    RUN_TEST(test_chmod_equal_x);
    RUN_TEST(test_chmod_ignore_symlink);
    RUN_TEST(test_chmod_silent);
    RUN_TEST(test_chmod_partial_fail);
    RUN_TEST(test_chmod_inaccessible);
    RUN_TEST(test_chmod_thru_dangling);
    RUN_TEST(test_chmod_symlinks);
    RUN_TEST(test_chmod_umask_x);
    RUN_TEST(test_chmod_only_op);
    RUN_TEST(test_chmod_c_option);
    RUN_TEST(test_chmod_octal);
    RUN_TEST(test_chmod_setgid);
    RUN_TEST(test_chmod_no_x);
    RUN_TEST(test_chmod_equals);
}
DEFINE_TEST_MAIN(chmod_suite)
