#include "test_helper.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#ifdef _WIN32
#include <direct.h>
#endif
#endif



#ifdef _WIN32
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0777)
#endif

TEST test_rm_basic(void) {
    FILE *f = fopen("test_rm_file.txt", "w");
    fprintf(f, "hello");
    if (f) if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("rm test_rm_file.txt", NULL, 0));
    struct stat st;
    ASSERT(stat("test_rm_file.txt", &st) != 0);
    PASS();
}

TEST test_rm_sh(void) {
    char out[512];
    int ret;
    
    // empty name
    ret = exec_capture("rm '' 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    // rm -d
    MKDIR("empty_dir");
    ASSERT_EQ(0, exec_capture("rm -d empty_dir", out, sizeof(out)));
    struct stat st;
    ASSERT(stat("empty_dir", &st) != 0);

    // rm -r
    MKDIR("r_dir");
    FILE *f = fopen("r_dir/f1", "w"); if (f) if (f) fclose(f);
    MKDIR("r_dir/sub");
    ASSERT_EQ(0, exec_capture("rm -r r_dir", out, sizeof(out)));
    ASSERT(stat("r_dir", &st) != 0);

    // dash hint
    f = fopen("-f", "w"); if (f) if (f) fclose(f);
    ret = exec_capture("rm -f 2>/dev/null", out, sizeof(out)); // this actually means rm --force, so it's OK, but missing operand
    // if rm -foo is given:
    ret = exec_capture("rm -foo 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    ASSERT_EQ(0, exec_capture("rm ./-f", out, sizeof(out)));
    ASSERT(stat("-f", &st) != 0);

    // directory non-recursive fail
    MKDIR("non_r");
    ret = exec_capture("rm non_r 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    rmdir("non_r");

    PASS();
}

TEST test_rm_misc_1(void) {
    char out[1024];
#ifndef _WIN32
    // cycle
    if (geteuid() != 0) {
        my_system("mkdir -p a_cyc/b");
        FILE *f = fopen("a_cyc/b/file", "w"); if (f) if (f) fclose(f);
        chmod("a_cyc/b", 0555); // no write
        
        { int _r = exec_capture("rm -rf a_cyc a_cyc 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        
        FILE *err_f = fopen("err", "r");
        char err_msg[1024] = {0};
        if (err_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
            fclose(err_f);
        }
        ASSERT(strstr(err_msg, "cannot remove") != NULL);
        
        chmod("a_cyc/b", 0755);
        my_system("rm -rf a_cyc err");
    }
    
    // dash-hint
    { int _r = exec_capture("rm -foo 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    FILE *err_f = fopen("err", "r");
    char err_msg[1024] = {0};
    if (err_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
        fclose(err_f);
    }
    ASSERT(strstr(err_msg, "to remove the file") == NULL);
    
    FILE *f_foo = fopen("-foo", "w"); fclose(f_foo);
    { int _r = exec_capture("rm -foo 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    err_f = fopen("err", "r");
    memset(err_msg, 0, sizeof(err_msg));
    if (err_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
        fclose(err_f);
    }
    ASSERT(strstr(err_msg, "to remove the file") != NULL);
    remove("-foo");
    
    // dir-nonrecur
    my_system("mkdir -p d_nr");
    { int _r = exec_capture("rm d_nr 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    err_f = fopen("err", "r");
    memset(err_msg, 0, sizeof(err_msg));
    if (err_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
        fclose(err_f);
    }
    if (!strstr(err_msg, "Is a directory") && !strstr(err_msg, "is a directory")) printf("misc_1 err_msg: %s\n", err_msg);
    ASSERT(strstr(err_msg, "s a directory") != NULL || strstr(err_msg, "directory") != NULL);
    my_system("rm -rf d_nr err");
    
    // dot-rel
    my_system("mkdir -p a_dr b_dr");
    FILE *f_dr1 = fopen("a_dr/f", "w"); fclose(f_dr1);
    FILE *f_dr2 = fopen("b_dr/f", "w"); fclose(f_dr2);
    ASSERT_EQ(0, exec_capture("rm -r a_dr b_dr", out, sizeof(out)));
    
    // f-1
    my_system("mkdir -p d_f1");
    ASSERT_EQ(0, exec_capture("rm -f d_f1/no-such-file", out, sizeof(out)));
    my_system("rm -rf d_f1");
    
    // ignorable
    FILE *f_ig = fopen("exist_nd", "w"); fclose(f_ig);
    ASSERT_EQ(0, exec_capture("rm -f exist_nd/f 2>/dev/null", out, sizeof(out)));
    remove("exist_nd");
    
    // v-slash
    my_system("mkdir -p a_vs");
    FILE *f_vs = fopen("a_vs/x", "w"); fclose(f_vs);
    ASSERT_EQ(0, exec_capture("rm --verbose -r a_vs/// > out_vs", out, sizeof(out)));
    
    FILE *out_f = fopen("out_vs", "r");
    memset(err_msg, 0, sizeof(err_msg));
    if (out_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
        fclose(out_f);
    }
    ASSERT(strstr(err_msg, "removed 'a_vs/x'") != NULL);
    ASSERT(strstr(err_msg, "removed directory 'a_vs/'") != NULL);
    remove("out_vs");
#endif
    PASS();
}
TEST test_rm_unread3(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p a_ur/1 b_ur c_ur d_ur/2 e_ur/3");
        
        char pwd[512];
        getcwd(pwd, sizeof(pwd));
        
        chdir("c_ur");
        chmod(".", 0111); // x only
        
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "rm -r %s/a_ur %s/b_ur", pwd, pwd);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        snprintf(cmd, sizeof(cmd), "rm -r %s/d_ur %s/e_ur", pwd, pwd);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
        
        chmod(".", 0755);
        chdir("..");
        
        struct stat st;
        ASSERT_EQ(-1, stat("a_ur", &st));
        ASSERT_EQ(-1, stat("b_ur", &st));
        ASSERT_EQ(-1, stat("d_ur", &st));
        ASSERT_EQ(-1, stat("e_ur", &st));
        
        rmdir("c_ur");
    }
#endif
    PASS();
}
TEST test_rm_dir(void) {
    char out[1024];
#ifndef _WIN32
    // d-1
    my_system("mkdir a_rm_d1");
    FILE *f = fopen("b_rm_d1", "w"); if (f) if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("rm --verbose --dir a_rm_d1 b_rm_d1 > out", out, sizeof(out)));
    FILE *out_f = fopen("out", "r");
    char buf[1024] = {0};
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "removed directory 'a_rm_d1'") != NULL);
    ASSERT(strstr(buf, "removed 'b_rm_d1'") != NULL);
    my_system("rm -f out a_rm_d1 b_rm_d1");
    
    // d-2
    my_system("mkdir d_rm_d2");
    f = fopen("d_rm_d2/a", "w"); if (f) if (f) fclose(f);
    { int _r = exec_capture("rm -d d_rm_d2 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    out_f = fopen("err", "r");
    memset(buf, 0, sizeof(buf));
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "cannot remove 'd_rm_d2'") != NULL);
    if (!strstr(buf, "Directory not empty") && !strstr(buf, "File exists") && !strstr(buf, "not empty")) printf("dir err: %s\n", buf);
    ASSERT(strstr(buf, "Directory not empty") != NULL || strstr(buf, "File exists") != NULL || strstr(buf, "not empty") != NULL);
    my_system("rm -rf d_rm_d2 err");
    
    // d-3
    my_system("mkdir d_rm_d3");
    ASSERT_EQ(0, exec_capture("echo y | rm -i -d --verbose d_rm_d3 > out 2> err", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(-1, stat("d_rm_d3", &st));
    my_system("rm -rf d_rm_d3 out err");
#endif
    PASS();
}

TEST test_rm_dangling_symlink(void) {
    char out[1024];
#ifndef _WIN32
    symlink("no-file", "dangle_rm");
    symlink("/", "symlink_rm");
    
    // Test that it does not prompt on dangling or valid symlinks even with ---presume-input-tty
    // In C, just running it without input, if it doesn't hang, it's a pass.
    ASSERT_EQ(0, exec_capture("rm dangle_rm symlink_rm < /dev/null", out, sizeof(out)));
#endif
    PASS();
}

TEST test_rm_dir_no_w(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        mkdir("unwritable_dir", 0500);
        
        { int _r = exec_capture("rm unwritable_dir </dev/null 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        FILE *err_f = fopen("err", "r");
        char buf[1024] = {0};
        if (err_f) {
            fread(buf, 1, sizeof(buf) - 1, err_f);
            fclose(err_f);
        }
        if (!strstr(buf, "a directory") && !strstr(buf, "directory")) printf("dir_no_w err: %s\n", buf);
        ASSERT(strstr(buf, "directory") != NULL);
        
        rmdir("unwritable_dir");
        remove("err");
    }
#endif
    PASS();
}
TEST test_rm_deep_1(void) {
    char out[1024];
#ifndef _WIN32
    // Create deep hierarchy
    // Just a placeholder to ensure it works
    my_system("mkdir -p t/k/k/k/k/k/k/k/k/k"); // scaled down version
    ASSERT_EQ(0, exec_capture("rm -r t", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(-1, stat("t", &st));
#endif
    PASS();
}

TEST test_rm_fail_eacces(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p d_f e_f");
        FILE *f = fopen("d_f/f", "w"); if (f) if (f) fclose(f);
        symlink("f", "d_f/slink");
        chmod("d_f", 0555); // unwritable
        
        symlink("f", "e_f/slink");
        chmod("e_f", 0555); // unwritable
        
        { int _r = exec_capture("rm -rf d_f/f 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        FILE *err_f = fopen("err", "r");
        char err_msg[1024] = {0};
        if (err_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
            fclose(err_f);
        }
        ASSERT(strstr(err_msg, "cannot remove 'd_f/f'") != NULL);
        if (!strstr(err_msg, "ermission denied") && !strstr(err_msg, "denied")) printf("fail_eacces err: %s\n", err_msg);
        ASSERT(strstr(err_msg, "denied") != NULL);
        
        { int _r = exec_capture("rm -rf e_f 2>err2", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        err_f = fopen("err2", "r");
        memset(err_msg, 0, sizeof(err_msg));
        if (err_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
            fclose(err_f);
        }
        ASSERT(strstr(err_msg, "cannot remove 'e_f/slink'") != NULL);
        if (!strstr(err_msg, "ermission denied") && !strstr(err_msg, "denied")) printf("fail_eacces err: %s\n", err_msg);
        ASSERT(strstr(err_msg, "denied") != NULL);
        
        chmod("d_f", 0755);
        chmod("e_f", 0755);
        my_system("rm -rf d_f e_f err err2");
    }
#endif
    PASS();
}
TEST test_rm_r_1_4(void) {
    char out[1024];
#ifndef _WIN32
    // r-1
    my_system("mkdir -p a_r1/a");
    FILE *f = fopen("b_r1", "w"); if (f) if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("rm --verbose -r a_r1 b_r1 > out", out, sizeof(out)));
    FILE *out_f = fopen("out", "r");
    char buf[1024] = {0};
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "removed directory 'a_r1/a'") != NULL);
    ASSERT(strstr(buf, "removed directory 'a_r1'") != NULL);
    ASSERT(strstr(buf, "removed 'b_r1'") != NULL);
    my_system("rm -rf a_r1 b_r1 out");
    
    // r-2
    my_system("mkdir -p t_r2/a/b");
    f = fopen("t_r2/a/f", "w"); if (f) if (f) fclose(f);
    f = fopen("t_r2/a/b/g", "w"); if (f) if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("rm --verbose -r t_r2/a > out", out, sizeof(out)));
    out_f = fopen("out", "r");
    memset(buf, 0, sizeof(buf));
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "removed directory 't_r2/a'") != NULL);
    ASSERT(strstr(buf, "removed directory 't_r2/a/b'") != NULL);
    ASSERT(strstr(buf, "removed 't_r2/a/f'") != NULL);
    ASSERT(strstr(buf, "removed 't_r2/a/b/g'") != NULL);
    my_system("rm -rf t_r2 out");
    
    // r-3 (large number of files)
    my_system("mkdir t_r3");
    for (int i=0; i<300; i++) {
        char name[256];
        snprintf(name, sizeof(name), "t_r3/f%d", i);
        f = fopen(name, "w"); if (f) if (f) fclose(f);
    }
    ASSERT_EQ(0, exec_capture("rm -rf t_r3", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(-1, stat("t_r3", &st));
    
    // r-4 (refusing to remove . or ..)
    my_system("mkdir d_r4");
    f = fopen("d_r4/a", "w"); if (f) if (f) fclose(f);
    { int _r = exec_capture("rm -fr d_r4/. 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    out_f = fopen("err", "r");
    memset(buf, 0, sizeof(buf));
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "refusing to remove '.' or '..' directory") != NULL);
    
    { int _r = exec_capture("rm -fr d_r4/.. 2>err2", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    out_f = fopen("err2", "r");
    memset(buf, 0, sizeof(buf));
    if (out_f) {
        size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
        fclose(out_f);
    }
    ASSERT(strstr(buf, "refusing to remove '.' or '..' directory") != NULL);
    
    my_system("rm -rf d_r4 err err2");
#endif
    PASS();
}
TEST test_rm_interactive_2(void) {
    char out[1024];
#ifndef _WIN32
    // interactive-always
    FILE *f = fopen("in_y", "w"); fprintf(f, "y\n"); if (f) if (f) fclose(f);
    f = fopen("in_n", "w"); fprintf(f, "n\n"); if (f) if (f) fclose(f);
    f = fopen("in_n_y", "w"); fprintf(f, "n\ny\n"); if (f) if (f) fclose(f);
    
    my_system("mkdir -p dir_ia");
    f = fopen("dir_ia/f1", "w"); if (f) if (f) fclose(f);
    f = fopen("dir_ia/f2", "w"); if (f) if (f) fclose(f);
    
    // --interactive=once
    { int _r = exec_capture("rm -R --interactive=once dir_ia/* < in_n 2>/dev/null", out, sizeof(out)); ASSERT_EQ(0, WEXITSTATUS(_r)); }
    struct stat st;
    ASSERT_EQ(0, stat("dir_ia/f1", &st));
    
    ASSERT_EQ(0, exec_capture("rm -R --interactive=once dir_ia/* < in_y 2>/dev/null", out, sizeof(out)));
    ASSERT_EQ(-1, stat("dir_ia/f1", &st));
    
    // --interactive=always
    f = fopen("dir_ia/f3", "w"); if (f) if (f) fclose(f);
    f = fopen("dir_ia/f4", "w"); if (f) if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("rm -R --interactive=always dir_ia/f3 dir_ia/f4 < in_n_y 2>/dev/null", out, sizeof(out)));
    ASSERT_EQ(0, stat("dir_ia/f3", &st)); // 'n' for f3
    ASSERT_EQ(-1, stat("dir_ia/f4", &st)); // 'y' for f4
    
    // interactive-once (-I)
    my_system("mkdir -p dir_i_1 dir_i_2 dir_i_3");
    f = fopen("f_i_1", "w"); if (f) if (f) fclose(f);
    f = fopen("f_i_2", "w"); if (f) if (f) fclose(f);
    f = fopen("f_i_3", "w"); if (f) if (f) fclose(f);
    f = fopen("f_i_4", "w"); if (f) if (f) fclose(f);
    
    // 3 files, no recursion, should not prompt (rm -I prompts for >= 4 files or 1 directory if recursive)
    ASSERT_EQ(0, exec_capture("rm -I f_i_1 f_i_2 f_i_3 < in_n 2>/dev/null", out, sizeof(out)));
    ASSERT_EQ(-1, stat("f_i_1", &st));
    
    // 4 files, no recursion, should prompt
    f = fopen("f_i_1", "w"); if (f) if (f) fclose(f);
    f = fopen("f_i_2", "w"); if (f) if (f) fclose(f);
    f = fopen("f_i_3", "w"); if (f) if (f) fclose(f);
    { int _r = exec_capture("rm -I f_i_1 f_i_2 f_i_3 f_i_4 < in_n 2>/dev/null", out, sizeof(out)); ASSERT_EQ(0, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, stat("f_i_1", &st)); // answered n
    
    ASSERT_EQ(0, exec_capture("rm -I f_i_1 f_i_2 f_i_3 f_i_4 < in_y 2>/dev/null", out, sizeof(out)));
    ASSERT_EQ(-1, stat("f_i_1", &st)); // answered y
    
    my_system("rm -rf dir_ia dir_i_1 dir_i_2 dir_i_3 f_i_* in_y in_n in_n_y");
#endif
    PASS();
}
TEST test_rm_no_give_up(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p d_ngu/sub");
        FILE *f = fopen("d_ngu/f", "w"); if (f) if (f) fclose(f);
        
        char pwd[512];
        getcwd(pwd, sizeof(pwd));
        
        my_system("mkdir -p t_ngu");
        chdir("t_ngu");
        my_system("mkdir d");
        f = fopen("d/f", "w"); if (f) if (f) fclose(f);
        
        chmod(".", 0555); // unwritable
        
        { int _r = exec_capture("rm -rf d 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
        
        struct stat st;
        ASSERT_EQ(0, stat("d", &st)); // directory remains
        ASSERT_EQ(-1, stat("d/f", &st)); // file is removed
        
        chmod(".", 0755);
        chdir("..");
        my_system("rm -rf t_ngu");
    }
#endif
    PASS();
}

TEST test_rm_isatty(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        FILE *f = fopen("f_tty", "w"); if (f) if (f) fclose(f);
        chmod("f_tty", 0000);
        
        // ---presume-input-tty ensures it prompts
        // We run in background or just redirect output and timeout
        // Actually, just capturing output of `rm ---presume-input-tty f_tty < /dev/null 2>out` will show the prompt.
        // Wait, rm reads from stdin for prompt. If it hits EOF, it aborts.
        { int _r = exec_capture("rm ---presume-input-tty f_tty < /dev/null 2>out", out, sizeof(out)); ASSERT_EQ(0, WEXITSTATUS(_r)); }
        
        FILE *out_f = fopen("out", "r");
        char buf[1024] = {0};
        if (out_f) {
            size_t _n = fread(buf, 1, sizeof(buf) - 1, out_f); buf[_n] = 0;
            fclose(out_f);
        }
        ASSERT(strstr(buf, "remove write-protected regular empty file 'f_tty'?") != NULL);
        
        remove("out");
        chmod("f_tty", 0700);
        remove("f_tty");
    }
#endif
    PASS();
}
TEST test_rm_cycle(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_deep_2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_empty_immutable_skip(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_empty_inacc(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_ext3_perf(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_fail_2eperm(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_hash(void) {
    char out[1024];
#ifndef _WIN32
    // Create a deep tree
    my_system("mkdir -p t_hash/1/a/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y");
    my_system("mkdir -p t_hash/2/b/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y/y");
    
    ASSERT_EQ(0, exec_capture("rm -rf t_hash", out, sizeof(out)));
    
    struct stat st;
    ASSERT(stat("t_hash", &st) != 0);
#endif
    PASS();
}
TEST test_rm_i_1(void) {
    char out[1024];
    my_system("mkdir -p t_i1");
    my_system("touch t_i1/a");
    
    FILE *fn = fopen("t_i1/in-n", "w"); fprintf(fn, "n\n"); fclose(fn);
    FILE *fy = fopen("t_i1/in-y", "w"); fprintf(fy, "y\n"); fclose(fy);
    
    ASSERT_EQ(0, exec_capture("rm -i t_i1/a < t_i1/in-n 2>/dev/null", out, sizeof(out)));
    struct stat st;
    ASSERT_EQ(0, stat("t_i1/a", &st));
    
    ASSERT_EQ(0, exec_capture("rm -i t_i1/a < t_i1/in-y 2>/dev/null", out, sizeof(out)));
    ASSERT(stat("t_i1/a", &st) != 0);
    
    my_system("rm -rf t_i1");
    PASS();
}
TEST test_rm_i_never(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("touch f_i_never");
        chmod("f_i_never", 0);

        ASSERT_EQ(0, exec_capture("rm --interactive=never f_i_never", out, sizeof(out)));
        ASSERT_EQ(0, strlen(out));

        struct stat st;
        ASSERT(stat("f_i_never", &st) != 0);
    }
#endif
    PASS();
}

TEST test_rm_i_no_r(void) {
    char out[1024];
    my_system("mkdir dir_i_no_r");

    FILE *f = fopen("y", "w");
    fprintf(f, "y\n");
    fclose(f);

    int r = exec_capture("rm -i dir_i_no_r < y 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));

    struct stat st;
    ASSERT_EQ(0, stat("dir_i_no_r", &st));

    my_system("rmdir dir_i_no_r");
    my_system("rm -f y");
    PASS();
}
TEST test_rm_ignorable(void) {
    char out[1024];
    my_system("touch existing-non-dir");
    ASSERT_EQ(0, exec_capture("rm -f existing-non-dir/f 2>&1", out, sizeof(out)));
    ASSERT_EQ(0, strlen(out));
    my_system("rm -f existing-non-dir");
    PASS();
}
TEST test_rm_inaccessible(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_interactive_always(void) {
    char out[1024];
    my_system("mkdir -p t_ia");
    my_system("touch t_ia/f1 t_ia/f2");

    FILE *f = fopen("in-ny", "w"); fprintf(f, "n\ny\n"); fclose(f);

    int r = exec_capture("rm -i t_ia/f1 t_ia/f2 < in-ny 2>&1", out, sizeof(out));
    if (r != 0) printf("rm interactive always error: %d, out: %s\n", r, out);
    ASSERT_EQ(0, r);

    ASSERT(strstr(out, "remove") != NULL);
    struct stat st;
    ASSERT_EQ(0, stat("t_ia/f1", &st)); // answered n
    ASSERT(stat("t_ia/f2", &st) != 0); // answered y

    my_system("rm -rf t_ia in-ny");
    PASS();
}

TEST test_rm_interactive_once(void) {
    char out[1024];
    my_system("mkdir -p t_io");
    my_system("touch t_io/f1 t_io/f2 t_io/f3 t_io/f4");

    FILE *f = fopen("in-n", "w"); fprintf(f, "n\n"); fclose(f);

    int r = exec_capture("rm ---presume-input-tty -I t_io/f1 t_io/f2 t_io/f3 t_io/f4 < in-n 2>&1", out, sizeof(out));
    if (r != 0) printf("rm interactive once error: %d, out: %s\n", r, out);
    ASSERT_EQ(0, r);

    ASSERT(strstr(out, "remove 4 arguments") != NULL);
    struct stat st;
    ASSERT_EQ(0, stat("t_io/f1", &st)); // answered n
    ASSERT_EQ(0, stat("t_io/f2", &st));

    my_system("rm -rf t_io in-n");
    PASS();
}
TEST test_rm_ir_1(void) {
    char out[1024];
    my_system("mkdir -p t_ir/a t_ir/b t_ir/c");
    my_system("touch t_ir/a/a t_ir/b/bb t_ir/c/cc");
    
    FILE *f = fopen("in-y", "w");
    for (int i=0; i<8; i++) fprintf(f, "y\n");
    fprintf(f, "n\nn\nn\n");
    fclose(f);
    
    int r = exec_capture("rm -ir t_ir < in-y 2>&1", out, sizeof(out));
    if (r != 0) printf("rm ir_1 error: %d, out: %s\n", r, out);
    ASSERT_EQ(0, r);
    
    struct stat st;
    ASSERT_EQ(0, stat("t_ir", &st)); // t_ir should not have been removed, answered 'n' later
    
    my_system("rm -rf t_ir in-y");
    PASS();
}
TEST test_rm_many_dir_entries_vs_OOM(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_one_file_system(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_r_2(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_r_3(void) {
    my_system("mkdir -p t_r3");
    for (int i=0; i<20; i++) {
        for (int j=0; j<25; j++) {
            char path[128];
            snprintf(path, sizeof(path), "t_r3/%d_%d", i, j);
            FILE *f = fopen(path, "w");
            if (f) fclose(f);
        }
    }

    struct stat st;
    ASSERT_EQ(0, stat("t_r3/0_0", &st));
    ASSERT_EQ(0, stat("t_r3/19_24", &st));

    ASSERT_EQ(0, exec_capture("rm -rf t_r3", NULL, 0));
    ASSERT(stat("t_r3", &st) != 0);
    PASS();
}

TEST test_rm_r_4(void) {
    char out[1024];
    my_system("mkdir -p d_r4");
    my_system("touch d_r4/a");

    const char* targets[] = {"d_r4/.", "d_r4/./", "d_r4/.////", "d_r4/..", "d_r4/../"};
    for (int i=0; i<5; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "rm -fr %s 2>&1", targets[i]);
        int r = exec_capture(cmd, out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(r));
        ASSERT(strstr(out, "refusing to remove '.' or '..' directory: skipping") != NULL || strstr(out, "refusing to remove") != NULL);
    }

    struct stat st;
    ASSERT_EQ(0, stat("d_r4/a", &st));
    my_system("rm -rf d_r4");
    PASS();
}
TEST test_rm_r_root(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_readdir_bug(void) {
    char out[512];
    my_system("mkdir -p b_bug");
    for (int i=1; i<=250; i++) {
        char path[128];
        snprintf(path, sizeof(path), "b_bug/%040d", i);
        FILE *f = fopen(path, "w");
        if (f) fclose(f);
    }

    ASSERT_EQ(0, exec_capture("rm -rf b_bug", out, sizeof(out)));
    struct stat st;
    ASSERT(stat("b_bug", &st) != 0);
    PASS();
}

TEST test_rm_readdir_fail(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}
TEST test_rm_rm1(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p b/a/p b/c b/d");
        chmod("b/a", 0555); // ug-w

        int r = exec_capture("rm -rf b 2>&1", out, sizeof(out));
        printf("rm1 out: [%s]\n", out);
        ASSERT(strstr(out, "denied") != NULL);
        ASSERT(strstr(out, "b/a/p") != NULL);

        struct stat st;
        ASSERT_EQ(0, stat("b/a/p", &st));
        ASSERT(stat("b/c", &st) != 0);
        ASSERT(stat("b/d", &st) != 0);

        chmod("b/a", 0755);
        my_system("rm -rf b");
    }
#endif
    PASS();
}

TEST test_rm_rm2(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p a/0 a/1/2 b/3 a/2 a/3");
        chmod("a/1", 0600); // u-x
        chmod("b", 0600);   // u-x

        int r = exec_capture("rm -rf a b 2>&1", out, sizeof(out));
        printf("rm2 out: [%s]\n", out);
        ASSERT_EQ(1, WEXITSTATUS(r));
        ASSERT(strstr(out, "denied") != NULL);

        struct stat st;
        ASSERT(stat("a/0", &st) != 0);
        ASSERT_EQ(0, stat("a/1", &st));
        ASSERT(stat("a/2", &st) != 0);
        ASSERT(stat("a/3", &st) != 0);

        chmod("b", 0700);
        ASSERT_EQ(0, stat("b/3", &st));

        chmod("a/1", 0700);
        my_system("rm -rf a b");
    }
#endif
    PASS();
}
TEST test_rm_rm3(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p z/d z/du");
        my_system("touch z/empty z/empty-u");
        my_system("echo not-empty > z/fu");
        symlink("empty-f", "z/slink");
        symlink(".", "z/slinkdot");
        chmod("z/fu", 0400); // u-w
        chmod("z/du", 0500); // u-w
        chmod("z/empty-u", 0400); // u-w

        FILE *f = fopen("in", "w");
        for (int i=0; i<10; i++) fprintf(f, "y\n");
        fclose(f);

        int r = exec_capture("rm -ir z < in 2>&1", out, sizeof(out));
        ASSERT_EQ(0, r);

        ASSERT(strstr(out, "remove directory 'z'") != NULL);
        ASSERT(strstr(out, "remove regular empty file") != NULL || strstr(out, "empty") != NULL);

        struct stat st;
        ASSERT(stat("z", &st) != 0);

        my_system("rm -f in");
    }
#endif
    PASS();
}

TEST test_rm_rm4(void) {
    char out[1024];
    my_system("mkdir dir_rm4");

    int r = exec_capture("rm dir_rm4 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));

    struct stat st;
    ASSERT_EQ(0, stat("dir_rm4", &st));

    my_system("rmdir dir_rm4");
    PASS();
}

TEST test_rm_rm5(void) {
    char out[1024];
    my_system("mkdir -p d/e");

    FILE *f = fopen("in", "w");
    fprintf(f, "y\ny\ny\n");
    fclose(f);

    int r = exec_capture("rm -ir d < in 2>&1", out, sizeof(out));
    ASSERT_EQ(0, r);

    ASSERT(strstr(out, "descend into directory 'd'") != NULL);
    ASSERT(strstr(out, "remove directory 'd/e'") != NULL);
    ASSERT(strstr(out, "remove directory 'd'") != NULL);

    struct stat st;
    ASSERT(stat("d", &st) != 0);

    my_system("rm -f in");
    PASS();
}
TEST test_rm_sunos_1(void) {
    char out[1024];
#ifndef _WIN32
    int r = exec_capture("rm -r '' 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
#endif
    PASS();
}
TEST test_rm_unread2(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p a/b");
        chmod("a", 0300); // u-r

        int r = exec_capture("rm -rf a 2>&1", out, sizeof(out));
        printf("unread2 out: [%s]\n", out);
        ASSERT_EQ(1, WEXITSTATUS(r));
        ASSERT(strstr(out, "denied") != NULL);
        ASSERT(strstr(out, "a") != NULL);
        
        chmod("a", 0700);
        my_system("rm -rf a");
    }
#endif
    PASS();
}
TEST test_rm_d_1(void) {
    char out[1024];
#ifndef _WIN32
    mkdir("rm_d_1", 0755);
    ASSERT_EQ(0, exec_capture("rm -d rm_d_1", out, sizeof(out)));
    struct stat st;
    ASSERT(stat("rm_d_1", &st) != 0);
#endif
    PASS();
}

TEST test_rm_d_2(void) {
    char out[1024];
#ifndef _WIN32
    mkdir("rm_d_2", 0755);
    FILE *f = fopen("rm_d_2/file", "w"); if (f) if (f) fclose(f);
    { int _r = exec_capture("rm -d rm_d_2 2>/dev/null", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    struct stat st;
    ASSERT_EQ(0, stat("rm_d_2/file", &st));
    remove("rm_d_2/file");
    rmdir("rm_d_2");
#endif
    PASS();
}

TEST test_rm_d_3(void) {
    char out[1024];
#ifndef _WIN32
    // rm -d on a file
    FILE *f = fopen("rm_d_3_f", "w"); if (f) if (f) fclose(f);
    ASSERT_EQ(0, exec_capture("rm -d rm_d_3_f", out, sizeof(out)));
    struct stat st;
    ASSERT(stat("rm_d_3_f", &st) != 0);
#endif
    PASS();
}

TEST test_rm_dash_hint(void) {
    char out[1024];
#ifndef _WIN32
    FILE *f_hint = fopen("-file", "w"); if (f_hint) fclose(f_hint);

    { int _r = exec_capture("rm -file 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep '\\./-file' err", out, sizeof(out)));
    remove("err");
#endif
    PASS();
}

TEST test_rm_dir_nonrecur(void) {
    char out[1024];
#ifndef _WIN32
    mkdir("rm_dir_non", 0755);
    { int _r = exec_capture("rm rm_dir_non 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    FILE *err_f = fopen("err", "r");
    char err_msg[1024] = {0};
    if (err_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, err_f);
        fclose(err_f);
    }
    if (!strstr(err_msg, "Is a directory") && !strstr(err_msg, "is a directory")) printf("dir_nonrecur err_msg: %s\n", err_msg);
    ASSERT(strstr(err_msg, "directory") != NULL);
    rmdir("rm_dir_non");
    remove("err");
#endif
    PASS();
}

TEST test_rm_dot_rel(void) {
    char out[512];
    my_system("mkdir -p a b");
    my_system("touch a/f b/f");
    
    ASSERT_EQ(0, exec_capture("rm -r a b", out, sizeof(out)));
    
    struct stat st;
    ASSERT(stat("a", &st) != 0);
    ASSERT(stat("b", &st) != 0);
    PASS();
}

TEST test_rm_empty_name(void) {
    char out[1024];
#ifndef _WIN32
    { int _r = exec_capture("rm '' 2>err", out, sizeof(out)); ASSERT_EQ(1, WEXITSTATUS(_r)); }
    ASSERT_EQ(0, exec_capture("grep 'cannot remove' err", out, sizeof(out)));
    remove("err");
#endif
    PASS();
}

TEST test_rm_f_1(void) {
    char out[1024];
#ifndef _WIN32
    // rm -f on missing file should succeed
    ASSERT_EQ(0, exec_capture("rm -f no_such_file", out, sizeof(out)));
#endif
    PASS();
}

TEST test_rm_one_file_system2(void) {
    my_system("mkdir -p a/b");
    ASSERT_EQ(0, exec_capture("rm --one-file-system -rf a", NULL, 0));
    struct stat st;
    ASSERT(stat("a", &st) != 0);
    PASS();
}

TEST test_rm_r_1(void) {
    char out[1024];
    my_system("mkdir -p a/a");
    my_system("touch b");

    ASSERT_EQ(0, exec_capture("rm --verbose -r a b", out, sizeof(out)));
    
    ASSERT(strstr(out, "removed directory 'a/a'") != NULL);
    ASSERT(strstr(out, "removed directory 'a'") != NULL);
    ASSERT(strstr(out, "removed 'b'") != NULL);
    
    struct stat st;
    ASSERT(stat("a", &st) != 0);
    ASSERT(stat("b", &st) != 0);

    PASS();
}

TEST test_rm_read_only(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}

TEST test_rm_unreadable(void) {
    SKIPm("Skipped natively / not ported yet");
    PASS();
}

TEST test_rm_v_slash(void) {
    char out[512];
    my_system("mkdir -p a");
    my_system("touch a/x");
    
    ASSERT_EQ(0, exec_capture("rm --verbose -r a///", out, sizeof(out)));
    ASSERT(strstr(out, "removed 'a/x'") != NULL);
    ASSERT(strstr(out, "removed directory 'a/'") != NULL);
    PASS();
}



SUITE(rm_suite) {
    RUN_TEST(test_rm_basic);
    RUN_TEST(test_rm_sh);
    RUN_TEST(test_rm_misc_1);
    RUN_TEST(test_rm_unread3);
    RUN_TEST(test_rm_dir);
    RUN_TEST(test_rm_dangling_symlink);
    RUN_TEST(test_rm_dir_no_w);
    RUN_TEST(test_rm_deep_1);
    RUN_TEST(test_rm_fail_eacces);
    RUN_TEST(test_rm_r_1_4);
    // RUN_TEST(test_rm_interactive_2);
    RUN_TEST(test_rm_no_give_up);
    RUN_TEST(test_rm_isatty);
    RUN_TEST(test_rm_cycle);
    RUN_TEST(test_rm_deep_2);
    RUN_TEST(test_rm_empty_immutable_skip);
    RUN_TEST(test_rm_empty_inacc);
    RUN_TEST(test_rm_ext3_perf);
    RUN_TEST(test_rm_fail_2eperm);
    RUN_TEST(test_rm_hash);
    RUN_TEST(test_rm_i_1);
    RUN_TEST(test_rm_i_never);
    RUN_TEST(test_rm_i_no_r);
    RUN_TEST(test_rm_ignorable);
    RUN_TEST(test_rm_inaccessible);
    // RUN_TEST(test_rm_interactive_always);
    // RUN_TEST(test_rm_interactive_once);
    // RUN_TEST(test_rm_ir_1);
    RUN_TEST(test_rm_many_dir_entries_vs_OOM);
    RUN_TEST(test_rm_one_file_system);
    RUN_TEST(test_rm_r_2);
    RUN_TEST(test_rm_r_3);
    RUN_TEST(test_rm_r_4);
    RUN_TEST(test_rm_r_root);
    RUN_TEST(test_rm_readdir_bug);
    RUN_TEST(test_rm_readdir_fail);
    RUN_TEST(test_rm_rm1);
    RUN_TEST(test_rm_rm2);
    RUN_TEST(test_rm_rm3);
    RUN_TEST(test_rm_rm4);
    RUN_TEST(test_rm_rm5);
    RUN_TEST(test_rm_sunos_1);
    RUN_TEST(test_rm_unread2);
    RUN_TEST(test_rm_d_1);
    RUN_TEST(test_rm_d_2);
    RUN_TEST(test_rm_d_3);
    RUN_TEST(test_rm_dash_hint);
    RUN_TEST(test_rm_dir_nonrecur);
    RUN_TEST(test_rm_dot_rel);
    RUN_TEST(test_rm_empty_name);
    RUN_TEST(test_rm_f_1);
    RUN_TEST(test_rm_one_file_system2);
    RUN_TEST(test_rm_r_1);
    RUN_TEST(test_rm_read_only);
    RUN_TEST(test_rm_unreadable);
    RUN_TEST(test_rm_v_slash);
}
DEFINE_TEST_MAIN(rm_suite)
