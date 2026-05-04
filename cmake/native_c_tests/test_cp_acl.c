#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    unlink("a/file");
    unlink("b/file");
    rmdir("a");
    rmdir("b");
}

TEST test_cp_acl(void) {
    cleanup();
    
    #ifdef _WIN32
    mkdir("a");
    mkdir("b");
    #else
    mkdir("a", 0700);
    mkdir("b", 0700);
    #endif

    FILE *f = fopen("a/file", "w");
    if (f) if (f) fclose(f);

    // Check if getfacl/setfacl are available
    char getfacl_out1[4096] = {0};
    int res = my_system("cd a && getfacl file > ../getfacl_out1 2>/dev/null");
    if (res != 0) {
        cleanup();
        SKIPm("getfacl not available or failed");
    }

    FILE *f_acl1 = fopen("getfacl_out1", "r");
    if (f_acl1) {
        fread(getfacl_out1, 1, sizeof(getfacl_out1)-1, f_acl1);
        fclose(f_acl1);
    }
    unlink("getfacl_out1");

    res = my_system("setfacl -m user:bin:rw- a/file >/dev/null 2>&1");
    if (res != 0) {
        res = my_system("setfacl -m u:bin:rw- a/file >/dev/null 2>&1");
        if (res != 0) {
            cleanup();
            SKIPm("setfacl not available or failed");
        }
    }

    // copy without preserving permissions
    res = exec_capture("cp a/file b/", NULL, 0);
    ASSERT_EQ(0, res);

    char getfacl_out2[4096] = {0};
    res = my_system("cd b && getfacl file > ../getfacl_out2 2>/dev/null");
    ASSERT_EQ(0, res);
    
    FILE *f_acl2 = fopen("getfacl_out2", "r");
    if (f_acl2) {
        fread(getfacl_out2, 1, sizeof(getfacl_out2)-1, f_acl2);
        fclose(f_acl2);
    }
    unlink("getfacl_out2");

    // Replace timestamps/variable fields? 
    // In acl.sh, they just do: test "$acl1" = "$acl2"
    // Wait, getfacl outputs the file name which differs if we do it in different dirs.
    // That's why acl.sh does `cd a && getfacl file` and `cd b && getfacl file`, so the file name in output is 'file' for both.
    ASSERT_STR_EQ(getfacl_out1, getfacl_out2);

    // Update acl1 with the modified ACL
    char getfacl_out1_mod[4096] = {0};
    res = my_system("cd a && getfacl file > ../getfacl_out1_mod 2>/dev/null");
    ASSERT_EQ(0, res);
    FILE *f_acl1_mod = fopen("getfacl_out1_mod", "r");
    if (f_acl1_mod) {
        fread(getfacl_out1_mod, 1, sizeof(getfacl_out1_mod)-1, f_acl1_mod);
        fclose(f_acl1_mod);
    }
    unlink("getfacl_out1_mod");

    // copy with preserving permissions
    res = exec_capture("cp -p a/file b/", NULL, 0);
    ASSERT_EQ(0, res);

    memset(getfacl_out2, 0, sizeof(getfacl_out2));
    res = my_system("cd b && getfacl file > ../getfacl_out2 2>/dev/null");
    ASSERT_EQ(0, res);
    f_acl2 = fopen("getfacl_out2", "r");
    if (f_acl2) {
        fread(getfacl_out2, 1, sizeof(getfacl_out2)-1, f_acl2);
        fclose(f_acl2);
    }
    unlink("getfacl_out2");

    ASSERT_STR_EQ(getfacl_out1_mod, getfacl_out2);

    // copy with preserving permissions and --attributes-only
    f = fopen("a/file", "w");
    if (f) {
        fprintf(f, "data\n");
        if (f) fclose(f);
    }
    
    struct stat st;
    stat("a/file", &st);
    ASSERT(st.st_size > 0);

    unlink("b/file"); // ensure it's removed before copying
    res = exec_capture("cp -p --attributes-only a/file b/", NULL, 0);
    ASSERT_EQ(0, res);

    stat("b/file", &st);
    ASSERT_EQ(0, st.st_size);

    memset(getfacl_out2, 0, sizeof(getfacl_out2));
    res = my_system("cd b && getfacl file > ../getfacl_out2 2>/dev/null");
    ASSERT_EQ(0, res);
    f_acl2 = fopen("getfacl_out2", "r");
    if (f_acl2) {
        fread(getfacl_out2, 1, sizeof(getfacl_out2)-1, f_acl2);
        fclose(f_acl2);
    }
    unlink("getfacl_out2");

    ASSERT_STR_EQ(getfacl_out1_mod, getfacl_out2);

    cleanup();
    PASS();
}





SUITE(cp_acl_suite) {
    RUN_TEST(test_cp_acl);
}
DEFINE_TEST_MAIN(cp_acl_suite)
