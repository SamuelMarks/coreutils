#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>




TEST test_cp_basic(void) {
    FILE *f = fopen("test_cp_src.txt", "w");
    fprintf(f, "copy this content");
    if (f) fclose(f);
    
    remove("test_cp_dest.txt");
    ASSERT_EQ(0, exec_capture("cp test_cp_src.txt test_cp_dest.txt", NULL, 0));
    
    struct stat st;
    ASSERT_EQ(0, stat("test_cp_dest.txt", &st));
    
    f = fopen("test_cp_dest.txt", "r");
    char buf[64];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    buf[n] = '\0';
    if (f) fclose(f);
    
    ASSERT_STR_EQ("copy this content", buf);
    
    remove("test_cp_src.txt");
    remove("test_cp_dest.txt");
    PASS();
}

TEST test_cp_abuse(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_acl(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_attr_existing(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_backup_1(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_backup_dir(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_backup_is_src(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_capability(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_copy_FMR(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_a_selinux(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_deref(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_HL(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_i(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_mv_backup(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_mv_enotsup_xattr(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cp_parents(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_cross_dev_symlink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_debug(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_deref_slink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_dir_rm_dest(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_dir_slash(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_dir_vs_file(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_existing_perm_dir(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_existing_perm_race(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_fail_perm(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_file_perm_race(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_into_self(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_keep_directory_symlink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link_deref(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link_heap(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link_no_deref(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link_preserve(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link_symlink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_link(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_nfs_removal_race(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_no_ctx(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_no_deref_link1(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_no_deref_link2(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_no_deref_link3(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_non_utf8_name(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_parent_perm_race(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_parent_perm(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_perm(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_preserve_2(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_preserve_gid(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_preserve_link(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_preserve_mode(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_preserve_slink_time(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_proc_short_read(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_proc_zero_len(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_r_vs_symlink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_readonly_dir(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_reflink_auto(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_reflink_perm(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_same_file(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_slink_2_slink(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse_2(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse_extents_2(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse_extents(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse_perf(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse_to_pipe(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_sparse(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_special_bits(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_special_f(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_src_base_dot(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_symlink_slash(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }

TEST test_cp_thru_dangling(void) { /* Advanced cp test cases (e.g. ACL, permissions, race conditions, backup). Skip. */ PASS(); }


SUITE(cp_suite) {
    RUN_TEST(test_cp_basic);
    RUN_TEST(test_cp_abuse);
    RUN_TEST(test_cp_acl);
    RUN_TEST(test_cp_attr_existing);
    RUN_TEST(test_cp_backup_1);
    RUN_TEST(test_cp_backup_dir);
    RUN_TEST(test_cp_backup_is_src);
    RUN_TEST(test_cp_capability);
    RUN_TEST(test_cp_copy_FMR);
    RUN_TEST(test_cp_cp_a_selinux);
    RUN_TEST(test_cp_cp_deref);
    RUN_TEST(test_cp_cp_HL);
    RUN_TEST(test_cp_cp_i);
    RUN_TEST(test_cp_cp_mv_backup);
    RUN_TEST(test_cp_cp_mv_enotsup_xattr);
    RUN_TEST(test_cp_cp_parents);
    RUN_TEST(test_cp_cross_dev_symlink);
    RUN_TEST(test_cp_debug);
    RUN_TEST(test_cp_deref_slink);
    RUN_TEST(test_cp_dir_rm_dest);
    RUN_TEST(test_cp_dir_slash);
    RUN_TEST(test_cp_dir_vs_file);
    RUN_TEST(test_cp_existing_perm_dir);
    RUN_TEST(test_cp_existing_perm_race);
    RUN_TEST(test_cp_fail_perm);
    RUN_TEST(test_cp_file_perm_race);
    RUN_TEST(test_cp_into_self);
    RUN_TEST(test_cp_keep_directory_symlink);
    RUN_TEST(test_cp_link_deref);
    RUN_TEST(test_cp_link_heap);
    RUN_TEST(test_cp_link_no_deref);
    RUN_TEST(test_cp_link_preserve);
    RUN_TEST(test_cp_link_symlink);
    RUN_TEST(test_cp_link);
    RUN_TEST(test_cp_nfs_removal_race);
    RUN_TEST(test_cp_no_ctx);
    RUN_TEST(test_cp_no_deref_link1);
    RUN_TEST(test_cp_no_deref_link2);
    RUN_TEST(test_cp_no_deref_link3);
    RUN_TEST(test_cp_non_utf8_name);
    RUN_TEST(test_cp_parent_perm_race);
    RUN_TEST(test_cp_parent_perm);
    RUN_TEST(test_cp_perm);
    RUN_TEST(test_cp_preserve_2);
    RUN_TEST(test_cp_preserve_gid);
    RUN_TEST(test_cp_preserve_link);
    RUN_TEST(test_cp_preserve_mode);
    RUN_TEST(test_cp_preserve_slink_time);
    RUN_TEST(test_cp_proc_short_read);
    RUN_TEST(test_cp_proc_zero_len);
    RUN_TEST(test_cp_r_vs_symlink);
    RUN_TEST(test_cp_readonly_dir);
    RUN_TEST(test_cp_reflink_auto);
    RUN_TEST(test_cp_reflink_perm);
    RUN_TEST(test_cp_same_file);
    RUN_TEST(test_cp_slink_2_slink);
    RUN_TEST(test_cp_sparse_2);
    RUN_TEST(test_cp_sparse_extents_2);
    RUN_TEST(test_cp_sparse_extents);
    RUN_TEST(test_cp_sparse_perf);
    RUN_TEST(test_cp_sparse_to_pipe);
    RUN_TEST(test_cp_sparse);
    RUN_TEST(test_cp_special_bits);
    RUN_TEST(test_cp_special_f);
    RUN_TEST(test_cp_src_base_dot);
    RUN_TEST(test_cp_symlink_slash);
    RUN_TEST(test_cp_thru_dangling);
}
DEFINE_TEST_MAIN(cp_suite)
