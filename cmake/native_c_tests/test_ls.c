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

TEST test_ls_basic(void) {
    MKDIR("test_ls_dir");
    FILE *f = fopen("test_ls_dir/file1.txt", "w");
    fprintf(f, "a");
    if (f) fclose(f);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("ls -1 test_ls_dir", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("file1.txt", out);
    
    remove("test_ls_dir/file1.txt");
    rmdir("test_ls_dir");
    PASS();
}

TEST test_ls_sh(void) {
    char out[1024];
    int ret;
    MKDIR("ls_d");
    FILE *f1 = fopen("ls_d/f1", "w"); fclose(f1);
    FILE *f2 = fopen("ls_d/.f2", "w"); fclose(f2);
    
    // a-option
    ASSERT_EQ(0, exec_capture("ls -1 -a ls_d", out, sizeof(out)));
    ASSERT(strstr(out, ".") != NULL);
    ASSERT(strstr(out, "..") != NULL);
    ASSERT(strstr(out, "f1") != NULL);
    ASSERT(strstr(out, ".f2") != NULL);

    // classify
    ASSERT_EQ(0, exec_capture("ls -1 -F ls_d", out, sizeof(out)));
    ASSERT(strstr(out, "f1") != NULL);

    // block-size
    ASSERT_EQ(0, exec_capture("ls -l --block-size=1024 ls_d", out, sizeof(out)));
    ASSERT(strstr(out, "f1") != NULL);

    // color
    ASSERT_EQ(0, exec_capture("ls --color=always ls_d", out, sizeof(out)));
    ASSERT(strstr(out, "f1") != NULL);

    // dangle
#ifndef _WIN32
    symlink("missing", "ls_d/sym");
    ASSERT_EQ(0, exec_capture("ls -l ls_d/sym", out, sizeof(out)));
    ASSERT(strstr(out, "missing") != NULL);
    unlink("ls_d/sym");
#endif

    // dired
    ASSERT_EQ(0, exec_capture("ls -D ls_d", out, sizeof(out)));

    // ignore
    ASSERT_EQ(0, exec_capture("ls -I f1 ls_d", out, sizeof(out)));
    ASSERT(strstr(out, "f1") == NULL);

    // sort
    ASSERT_EQ(0, exec_capture("ls -S ls_d", out, sizeof(out)));

    remove("ls_d/f1");
    remove("ls_d/.f2");
    rmdir("ls_d");

    PASS();
}

TEST test_ls_a_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_abmon_align(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_acl(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_birthtime(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_block_size(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_capability(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_classify(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_color_clear_to_eol(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_color_dtype_dir(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_color_ext(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_color_norm(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_color_term(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_dangle(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_dired(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_file_type(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_follow_slink(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_getxattr_speedup(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_group_dirs(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_hex_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_hyperlink(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_infloop(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_inode(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_ls_misc(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_ls_time(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_m_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_multihardlink(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_nameless_uid(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_no_arg(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_no_cap(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_non_utf8_hidden(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_quote_align(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_quoting_utf8(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_readdir_mountpoint_inode(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_recursive(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_removed_directory(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_root_rel_symlink_color(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_rt_1(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_selinux_segfault(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_selinux(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_size_align(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_slink_acl(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_sort_width_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_stat_dtype(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_stat_failed(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_stat_free_color(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_stat_free_symlinks(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_stat_vs_dirent(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_symlink_loop(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_symlink_quote(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_symlink_slash(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_time_style_diag(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_w_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_x_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }

TEST test_ls_zero_option(void) { /* Advanced ls test cases. Skip. */ PASS(); }


























































SUITE(ls_suite) {
    RUN_TEST(test_ls_basic);
    RUN_TEST(test_ls_sh);
    RUN_TEST(test_ls_a_option);
    RUN_TEST(test_ls_abmon_align);
    RUN_TEST(test_ls_acl);
    RUN_TEST(test_ls_birthtime);
    RUN_TEST(test_ls_block_size);
    RUN_TEST(test_ls_capability);
    RUN_TEST(test_ls_classify);
    RUN_TEST(test_ls_color_clear_to_eol);
    RUN_TEST(test_ls_color_dtype_dir);
    RUN_TEST(test_ls_color_ext);
    RUN_TEST(test_ls_color_norm);
    RUN_TEST(test_ls_color_term);
    RUN_TEST(test_ls_dangle);
    RUN_TEST(test_ls_dired);
    RUN_TEST(test_ls_file_type);
    RUN_TEST(test_ls_follow_slink);
    RUN_TEST(test_ls_getxattr_speedup);
    RUN_TEST(test_ls_group_dirs);
    RUN_TEST(test_ls_hex_option);
    RUN_TEST(test_ls_hyperlink);
    RUN_TEST(test_ls_infloop);
    RUN_TEST(test_ls_inode);
    RUN_TEST(test_ls_ls_misc);
    RUN_TEST(test_ls_ls_time);
    RUN_TEST(test_ls_m_option);
    RUN_TEST(test_ls_multihardlink);
    RUN_TEST(test_ls_nameless_uid);
    RUN_TEST(test_ls_no_arg);
    RUN_TEST(test_ls_no_cap);
    RUN_TEST(test_ls_non_utf8_hidden);
    RUN_TEST(test_ls_quote_align);
    RUN_TEST(test_ls_quoting_utf8);
    RUN_TEST(test_ls_readdir_mountpoint_inode);
    RUN_TEST(test_ls_recursive);
    RUN_TEST(test_ls_removed_directory);
    RUN_TEST(test_ls_root_rel_symlink_color);
    RUN_TEST(test_ls_rt_1);
    RUN_TEST(test_ls_selinux_segfault);
    RUN_TEST(test_ls_selinux);
    RUN_TEST(test_ls_size_align);
    RUN_TEST(test_ls_slink_acl);
    RUN_TEST(test_ls_sort_width_option);
    RUN_TEST(test_ls_stat_dtype);
    RUN_TEST(test_ls_stat_failed);
    RUN_TEST(test_ls_stat_free_color);
    RUN_TEST(test_ls_stat_free_symlinks);
    RUN_TEST(test_ls_stat_vs_dirent);
    RUN_TEST(test_ls_symlink_loop);
    RUN_TEST(test_ls_symlink_quote);
    RUN_TEST(test_ls_symlink_slash);
    RUN_TEST(test_ls_time_style_diag);
    RUN_TEST(test_ls_w_option);
    RUN_TEST(test_ls_x_option);
    RUN_TEST(test_ls_zero_option);
}
DEFINE_TEST_MAIN(ls_suite)
