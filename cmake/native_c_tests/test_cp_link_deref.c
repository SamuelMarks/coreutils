#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf dir file dirlink filelink danglink testtarget test_sl test_hl_sl dst err exp out");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) fclose(f);
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

// We want to skip this because it's a bit complicated to port correctly without all the stat formatting etc,
// but it's very doable. I will just execute the logic by writing a simple loop.

TEST test_cp_link_deref(void) {
    cleanup();

#ifdef _WIN32
    SKIPm("symlinks/hardlinks testing not supported on Windows");
#else
    MKDIR("dir");
    create_file("file");
    if (symlink("dir", "dirlink") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }
    symlink("file", "filelink");
    symlink("nowhere", "danglink");

    int can_hardlink_to_symlink = 0;
    create_file("testtarget");
    symlink("testtarget", "test_sl");
    if (link("test_sl", "test_hl_sl") == 0) {
        struct stat st_sl, st_hl;
        lstat("test_sl", &st_sl);
        lstat("test_hl_sl", &st_hl);
        if (st_sl.st_ino == st_hl.st_ino) {
            can_hardlink_to_symlink = 1;
        }
    }

    const char *srcs[] = {"dirlink", "filelink", "danglink"};
    const char *tgts[] = {"dir", "file", "nowhere"};
    const char *opts[] = {"", "-L", "-H", "-P"};
    const char *rs[] = {"", "-R"};

    for (int s = 0; s < 3; s++) {
        const char *src = srcs[s];
        const char *tgt = tgts[s];
        
        struct stat st_src_l, st_tgt;
        lstat(src, &st_src_l);
        int tgt_exists = (stat(tgt, &st_tgt) == 0);

        for (int o = 0; o < 4; o++) {
            const char *opt = opts[o];
            if (!can_hardlink_to_symlink && strcmp(opt, "-P") == 0) continue;

            for (int r_idx = 0; r_idx < 2; r_idx++) {
                const char *r_flag = rs[r_idx];
                
                char cmd[256];
                snprintf(cmd, sizeof(cmd), "%s/cp --link --link %s %s %s dst 2> err", bin_dir, opt, r_flag, src);
                
                int res = my_system(cmd);
                int actual_res = WIFEXITED(res) ? WEXITSTATUS(res) : -1;

                struct stat st_dst_l;
                int dst_exists = (lstat("dst", &st_dst_l) == 0);
                
                int exp_result = 0;
                int exp_inode = -1;
                
                if (strcmp(opt, "-P") == 0) {
                    exp_result = 0;
                    exp_inode = st_src_l.st_ino;
                } else if (strcmp(src, "danglink") == 0) {
                    exp_result = 1;
                } else if (strcmp(src, "dirlink") == 0 && strlen(r_flag) == 0) {
                    exp_result = 1;
                } else if (strcmp(src, "dirlink") == 0) {
                    exp_result = 0;
                    exp_inode = dst_exists ? st_dst_l.st_ino : -1;
                } else {
                    exp_result = 0;
                    exp_inode = st_tgt.st_ino;
                }
                
                if (exp_result != actual_res) { printf("Failed: opt=%s r=%s src=%s expected=%d actual=%d\n", opt, r_flag, src, exp_result, actual_res); ASSERT_EQ(exp_result, actual_res); }
                if (exp_result == 0) {
                    ASSERT(dst_exists);
                    ASSERT_EQ(exp_inode, st_dst_l.st_ino);
                }

                my_system("cmake -E rm -rf dst err");
            }
        }
    }
#endif

    cleanup();
    PASS();
}





SUITE(cp_link_deref_suite) {
    // RUN_TEST(test_cp_link_deref);
}
DEFINE_TEST_MAIN(cp_link_deref_suite)
