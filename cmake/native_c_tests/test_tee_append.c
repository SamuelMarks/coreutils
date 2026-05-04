#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>



TEST test_tee_append(void) {
    // Port of tests/tee/append.sh

    // echo 'line 1' > inp
    FILE *f = fopen("inp", "w");
    fprintf(f, "line 1\n");
    if (f) fclose(f);

    // cat inp > exp
    f = fopen("exp", "w");
    fprintf(f, "line 1\n");
    if (f) fclose(f);

    char out[1024];
    
    // tee 1 2 ... 13 < inp > out
    ASSERT_EQ(0, exec_capture("tee 1 2 3 4 5 6 7 8 9 10 11 12 13 < inp > out", out, sizeof(out)));

    // compare exp out
    ASSERT_EQ(0, exec_capture("cmp exp out", out, sizeof(out)));

    // compare exp for each file
    for (int i = 1; i <= 13; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cmp exp %d", i);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    }

    // echo 'line 2' > inp
    f = fopen("inp", "w");
    fprintf(f, "line 2\n");
    if (f) fclose(f);

    // cat inp >> exp
    f = fopen("exp", "a");
    fprintf(f, "line 2\n");
    if (f) fclose(f);

    // tee -a 1 2 ... 13 < inp > out
    ASSERT_EQ(0, exec_capture("tee -a 1 2 3 4 5 6 7 8 9 10 11 12 13 < inp > out", out, sizeof(out)));

    // compare inp out
    ASSERT_EQ(0, exec_capture("cmp inp out", out, sizeof(out)));

    // compare exp for each file
    for (int i = 1; i <= 13; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cmp exp %d", i);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    }

    // Clean up
    remove("inp");
    remove("exp");
    remove("out");
    for (int i = 1; i <= 13; i++) {
        char fname[16];
        snprintf(fname, sizeof(fname), "%d", i);
        remove(fname);
    }

    PASS();
}

TEST test_tee_append_long(void) {
    // Same test for --append
    FILE *f = fopen("inp", "w");
    fprintf(f, "line 1\n");
    if (f) fclose(f);

    f = fopen("exp", "w");
    fprintf(f, "line 1\n");
    if (f) fclose(f);

    char out[1024];
    
    ASSERT_EQ(0, exec_capture("tee 1 2 3 4 5 6 7 8 9 10 11 12 13 < inp > out", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cmp exp out", out, sizeof(out)));

    for (int i = 1; i <= 13; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cmp exp %d", i);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    }

    f = fopen("inp", "w");
    fprintf(f, "line 2\n");
    if (f) fclose(f);

    f = fopen("exp", "a");
    fprintf(f, "line 2\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("tee --append 1 2 3 4 5 6 7 8 9 10 11 12 13 < inp > out", out, sizeof(out)));
    ASSERT_EQ(0, exec_capture("cmp inp out", out, sizeof(out)));

    for (int i = 1; i <= 13; i++) {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "cmp exp %d", i);
        ASSERT_EQ(0, exec_capture(cmd, out, sizeof(out)));
    }

    remove("inp");
    remove("exp");
    remove("out");
    for (int i = 1; i <= 13; i++) {
        char fname[16];
        snprintf(fname, sizeof(fname), "%d", i);
        remove(fname);
    }

    PASS();
}





SUITE(tee_append_suite) {
    RUN_TEST(test_tee_append);
    RUN_TEST(test_tee_append_long);
}
DEFINE_TEST_MAIN(tee_append_suite)
