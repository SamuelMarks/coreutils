#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>



static void cleanup() {
    unlink("a/1");
    unlink("b/1");
    unlink("c/1");
    rmdir("a");
    rmdir("b");
    rmdir("c");
    unlink("t");
}

TEST test_cp_abuse(void) {
    cleanup();
    
    #ifdef _WIN32
    mkdir("a");
    mkdir("b");
    mkdir("c");
    #else
    mkdir("a", 0700);
    mkdir("b", 0700);
    mkdir("c", 0700);
    #endif

    if (symlink("../t", "a/1") != 0) {
        cleanup();
        SKIPm("symlink not supported");
    }

    FILE *f = fopen("b/1", "w");
    if (f) {
        fprintf(f, "payload\n");
        if (f) fclose(f);
    }

    const char *exp = "cp: will not copy 'b/1' through just-created symlink 'c/1'\n";

    const char *cases[] = {"dangling-dest", "existing-dest"};
    for (int i = 0; i < 2; i++) {
        unlink("c/1"); // Clean up dest
        if (strcmp(cases[i], "existing-dest") == 0) {
            FILE *ft = fopen("t", "w");
            if (ft) {
                fprintf(ft, "i\n");
                fclose(ft);
            }
        } else {
            unlink("t");
        }

        char out[1024];
        int res = exec_capture("cp -dR a/1 b/1 c", out, sizeof(out));
        ASSERT_NEQ(0, res);

        char *p = out;
        char normalized_out[1024];
        int j = 0;
        while (*p && j < sizeof(normalized_out) - 1) {
            if (*p != '\r') {
                normalized_out[j++] = *p;
            }
            p++;
        }
        normalized_out[j] = '\0';

        ASSERT_STR_EQ(exp, normalized_out);

        if (strcmp(cases[i], "dangling-dest") == 0) {
            struct stat st;
            ASSERT_EQ(-1, stat("t", &st)); // t should not exist
        } else {
            FILE *ft = fopen("t", "r");
            ASSERT(ft != NULL);
            char buf[16] = {0};
            fread(buf, 1, sizeof(buf) - 1, ft);
            fclose(ft);
            
            p = buf;
            char normalized_buf[16];
            j = 0;
            while (*p && j < sizeof(normalized_buf) - 1) {
                if (*p != '\r') {
                    normalized_buf[j++] = *p;
                }
                p++;
            }
            normalized_buf[j] = '\0';

            ASSERT_STR_EQ("i\n", normalized_buf);
        }
    }

    cleanup();
    PASS();
}





SUITE(cp_abuse_suite) {
    RUN_TEST(test_cp_abuse);
}
DEFINE_TEST_MAIN(cp_abuse_suite)
