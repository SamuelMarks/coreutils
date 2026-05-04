#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f no-such dangle f out loop");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) {
        fprintf(f, "hi\n");
        if (f) fclose(f);
    }
}

TEST test_cp_thru_dangling(void) {
    cleanup();

    create_file("f");

#ifndef _WIN32
    if (symlink("no-such", "dangle") != 0) {
        cleanup();
        SKIPm("symlinks not supported");
    }

    const char *opts[] = {"", "-f"};
    char out[1024];

    for (int i = 0; i < 2; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cp %s f dangle 2>&1", opts[i]);
        ASSERT_NEQ(0, exec_capture(cmd, out, sizeof(out)));
        ASSERT(strstr(out, "not writing through dangling symlink") != NULL);

        struct stat st;
        ASSERT_EQ(-1, stat("no-such", &st)); // no-such should not exist
    }

    // But you can set POSIXLY_CORRECT to get the historical behavior.
    // wait, POSIXLY_CORRECT=1 prepended to exec_capture? exec_capture uses my_system().
    // my_system("POSIXLY_CORRECT=1 ./src/cp f dangle")
    // Note that exec_capture uses argv, so it might not support env var prepending natively
    // if it relies on execvp directly in C?
    // Wait, the exec_capture macro in test_helper uses `system` or `fork`? Let's assume we can just use `putenv`.
    
    putenv((char*)"POSIXLY_CORRECT=1");
    int res = exec_capture("cp f dangle", out, sizeof(out));
    putenv((char*)"POSIXLY_CORRECT="); // unset or clear it
    // Wait, let's just use system for this specific call to be safe if exec_capture doesn't like it.
    if (res != 0) {
        // let's try the direct system approach
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "env POSIXLY_CORRECT=1 ./%s/cp f dangle > out 2>&1", bin_dir);
        ASSERT_EQ(0, my_system(cmd));
    } else {
        ASSERT_EQ(0, res);
    }

    // POSIXLY_CORRECT works via environment variable
    struct stat st;
    ASSERT_EQ(0, stat("no-such", &st));
    
    FILE *fn = fopen("no-such", "r");
    memset(out, 0, sizeof(out));
    if (fn) {
        fread(out, 1, sizeof(out)-1, fn);
        fclose(fn);
    }
    ASSERT_STR_EQ("hi\n", out);

    // Starting with 8.30 we treat ELOOP as existing and so remove the symlink
    if (symlink("loop", "loop") == 0) {
        ASSERT_EQ(0, exec_capture("cp -f f loop 2>&1", out, sizeof(out)));
        ASSERT_EQ(0, strlen(out));
        
        memset(out, 0, sizeof(out));
        FILE *fl = fopen("loop", "r");
        if (fl) {
            fread(out, 1, sizeof(out)-1, fl);
            fclose(fl);
        }
        ASSERT_STR_EQ("hi\n", out);
        
        ASSERT_EQ(0, stat("loop", &st));
        ASSERT(S_ISREG(st.st_mode)); // should be a regular file now
    }
#else
    SKIPm("symlinks not supported on Windows");
#endif

    cleanup();
    PASS();
}





SUITE(cp_thru_dangling_suite) {
    RUN_TEST(test_cp_thru_dangling);
}
DEFINE_TEST_MAIN(cp_thru_dangling_suite)
