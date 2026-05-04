#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f in in2 in2b out exp");
}

static void create_file_binary(const char *name, const char *content, size_t len) {
    FILE *f = fopen(name, "wb");
    if (f) {
        fwrite(content, 1, len, f);
        if (f) fclose(f);
    }
}

TEST test_cat_E(void) {
    cleanup();

    char out[1024] = {0};
    
    // Test 1: \r followed by \n is displayed as ^M$
    const char in1[] = "a\rb\r\nc\n\r\nd\r";
    const char exp1[] = "a\rb^M$\nc$\n^M$\nd\r";
    create_file_binary("in", in1, sizeof(in1)-1);
    
    ASSERT_EQ(0, exec_capture("cat -E in", out, sizeof(out)));
    ASSERT_STR_EQ(exp1, out);

    // Test 2: Ensure \r\n spanning files (or buffers) is handled
    const char in2[] = "1\r";
    const char in2b[] = "\n2\r\n";
    const char exp2[] = "1^M$\n2^M$\n";
    create_file_binary("in2", in2, sizeof(in2)-1);
    create_file_binary("in2b", in2b, sizeof(in2b)-1);

    memset(out, 0, sizeof(out));
    ASSERT_EQ(0, exec_capture("cat -E in2 in2b", out, sizeof(out)));
    ASSERT_STR_EQ(exp2, out);

    // Test 3: Ensure \r at end of buffer is handled
    const char in3[] = "1\r";
    const char in3b[] = "2\r\n";
    const char exp3[] = "1\r2^M$\n";
    create_file_binary("in2", in3, sizeof(in3)-1);
    create_file_binary("in2b", in3b, sizeof(in3b)-1);

    memset(out, 0, sizeof(out));
    ASSERT_EQ(0, exec_capture("cat -E in2 in2b", out, sizeof(out)));
    ASSERT_STR_EQ(exp3, out);

    cleanup();
    PASS();
}





SUITE(cat_E_suite) {
    RUN_TEST(test_cat_E);
}
DEFINE_TEST_MAIN(cat_E_suite)
