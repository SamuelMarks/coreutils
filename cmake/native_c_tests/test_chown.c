#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>





TEST test_chown_basic(void) {
    char out[512];
    ASSERT_EQ(0, exec_capture("chown --version", out, sizeof(out)));
    ASSERT(strstr(out, "chown") != NULL);
    PASS();
}

TEST test_chown_fail(void) {
    char out[512];
    int ret;
    
    FILE *f = fopen("f", "w"); if (f) fclose(f);
    
    // Non-existent user
    ret = exec_capture("chown invalid_user f 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));

    char cmd[512];
    char user[128];
    exec_capture("id -un", user, sizeof(user));
    trim_newline(user);
    
    snprintf(cmd, sizeof(cmd), "chown %s f", user);
    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));

    remove("f");
    PASS();
}

TEST test_chown_deref(void) {
    char out[1024];
#ifndef _WIN32
    symlink("no-such", "dangle_chown");
    
    char user[128];
    exec_capture("id -un", user, sizeof(user));
    trim_newline(user);
    
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "chown --dereference %s dangle_chown 2>out", user);
    int ret = exec_capture(cmd, out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(ret));
    
    FILE *out_f = fopen("out", "r");
    char err_msg[1024] = {0};
    if (out_f) {
        fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
        fclose(out_f);
    }
    ASSERT(strstr(err_msg, "cannot dereference") != NULL);
    
    remove("out");
    unlink("dangle_chown");
#endif
    PASS();
}

TEST test_chown_preserve_root(void) {
    char out[1024];
#ifndef _WIN32
    if (geteuid() != 0) {
        my_system("mkdir -p d_root");
        symlink("/", "d_root/slink-to-root");
        
        int ret = exec_capture("chown -R --preserve-root 0 / 2>out", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        
        FILE *out_f = fopen("out", "r");
        char err_msg[1024] = {0};
        if (out_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
            fclose(out_f);
        }
        ASSERT(strstr(err_msg, "dangerous to operate recursively on '/'") != NULL);
        
        // Also test with Lh
        ret = exec_capture("chown -RLh --preserve-root 0 d_root 2>out2", out, sizeof(out));
        ASSERT_EQ(1, WEXITSTATUS(ret));
        out_f = fopen("out2", "r");
        memset(err_msg, 0, sizeof(err_msg));
        if (out_f) {
            fread(err_msg, 1, sizeof(err_msg) - 1, out_f);
            fclose(out_f);
        }
        ASSERT(strstr(err_msg, "dangerous to operate recursively on") != NULL);
        
        remove("out");
        remove("out2");
        unlink("d_root/slink-to-root");
        rmdir("d_root");
    }
#endif
    PASS();
}

TEST test_chown_separator(void) {
    char out[1024];
#ifndef _WIN32
    char uid[32], user[128], gid[32], group[128];
    exec_capture("id -u", uid, sizeof(uid)); trim_newline(uid);
    exec_capture("id -un", user, sizeof(user)); trim_newline(user);
    exec_capture("id -g", gid, sizeof(gid)); trim_newline(gid);
    exec_capture("id -gn", group, sizeof(group)); trim_newline(group);
    
    // Test empty chown
    ASSERT_EQ(0, exec_capture("chown '' .", out, sizeof(out)));
    
    char *us[] = {uid, user, ""};
    char *gs[] = {gid, group, ""};
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char cmd[256];
            snprintf(cmd, sizeof(cmd), "chown %s:%s .", us[i], gs[j]);
            
            // if both are empty or trailing colon, handle special cases
            if (strlen(us[i]) == 0 && strlen(gs[j]) == 0) {
                // `chown : .` is valid, changes nothing or group? `chown : .` works
            }
            
            // Just test one valid combination to prove it works
            if (strlen(us[i]) > 0 && strlen(gs[j]) > 0) {
                snprintf(cmd, sizeof(cmd), "chown %s:%s .", us[i], gs[j]);
                ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
                
                // Dot separator
                if (strchr(us[i], '.') == NULL && strchr(gs[j], '.') == NULL) {
                    snprintf(cmd, sizeof(cmd), "chown %s.%s .", us[i], gs[j]);
                    ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
                }
            }
        }
    }
#endif
    PASS();
}



SUITE(chown_suite) {
    RUN_TEST(test_chown_basic);
    RUN_TEST(test_chown_fail);
    RUN_TEST(test_chown_deref);
    RUN_TEST(test_chown_preserve_root);
    RUN_TEST(test_chown_separator);
}
DEFINE_TEST_MAIN(chown_suite)
