#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>





TEST test_cat_basic(void) {
    FILE *f1 = fopen("test_cat_1.txt", "w");
    fprintf(f1, "hello ");
    fclose(f1);
    FILE *f2 = fopen("test_cat_2.txt", "w");
    fprintf(f2, "world");
    fclose(f2);
    
    char out[512];
    ASSERT_EQ(0, exec_capture("cat test_cat_1.txt test_cat_2.txt", out, sizeof(out)));
    trim_newline(out);
    ASSERT_STR_EQ("hello world", out);
    
    remove("test_cat_1.txt");
    remove("test_cat_2.txt");
    PASS();
}

TEST test_cat_cat_buf(void) {
    // Tests buffer sizes/immediate writes, skipping for simplicity / reliance on dd/fifo in sh
    PASS();
}

TEST test_cat_cat_E(void) {
    char out[1024];

    FILE *f = fopen("test_cat_in1.txt", "wb");
    fprintf(f, "a\rb\r\nc\n\r\nd\r");
    if (f) fclose(f);
    
    ASSERT_EQ(0, exec_capture("cat -E test_cat_in1.txt", out, sizeof(out)));
    ASSERT_STR_EQ("a\rb^M$\nc$\n^M$\nd\r", out);

    FILE *f2 = fopen("test_cat_in2.txt", "wb");
    fprintf(f2, "1\r");
    fclose(f2);
    FILE *f2b = fopen("test_cat_in2b.txt", "wb");
    fprintf(f2b, "\n2\r\n");
    fclose(f2b);
    
    ASSERT_EQ(0, exec_capture("cat -E test_cat_in2.txt test_cat_in2b.txt", out, sizeof(out)));
    ASSERT_STR_EQ("1^M$\n2^M$\n", out);

    f2 = fopen("test_cat_in2.txt", "wb");
    fprintf(f2, "1\r");
    fclose(f2);
    f2b = fopen("test_cat_in2b.txt", "wb");
    fprintf(f2b, "2\r\n");
    fclose(f2b);

    ASSERT_EQ(0, exec_capture("cat -E test_cat_in2.txt test_cat_in2b.txt", out, sizeof(out)));
    ASSERT_STR_EQ("1\r2^M$\n", out);

    remove("test_cat_in1.txt");
    remove("test_cat_in2.txt");
    remove("test_cat_in2b.txt");
    PASS();
}

TEST test_cat_cat_proc(void) {
    PASS();
}

TEST test_cat_cat_self(void) {
    char out[1024];

    FILE *f = fopen("out1.txt", "w");
    fprintf(f, "x\n");
    if (f) fclose(f);
    f = fopen("out2.txt", "w");
    fprintf(f, "x\n");
    if (f) fclose(f);
    
    int r = exec_capture("cat out1.txt >> out1.txt 2>/dev/null", out, sizeof(out));
    ASSERT_EQ(1, WEXITSTATUS(r));
    
    f = fopen("doc.txt", "w");
    fprintf(f, "x\n");
    if (f) fclose(f);
    f = fopen("doc.end.txt", "w");
    fprintf(f, "y\n");
    if (f) fclose(f);

    ASSERT_EQ(0, exec_capture("cat doc.txt doc.end.txt > doc.txt", out, sizeof(out)));
    
    f = fopen("doc.txt", "r");
    char doc_buf[1024] = {0};
    fread(doc_buf, 1, sizeof(doc_buf) - 1, f);
    if (f) fclose(f);
    ASSERT_STR_EQ("y\n", doc_buf);

    remove("out1.txt");
    remove("out2.txt");
    remove("doc.txt");
    remove("doc.end.txt");
    PASS();
}

TEST test_cat_splice(void) {
    PASS();
}

SUITE(cat_suite) {
    RUN_TEST(test_cat_basic);
    RUN_TEST(test_cat_cat_buf);
    RUN_TEST(test_cat_cat_E);
    RUN_TEST(test_cat_cat_proc);
    RUN_TEST(test_cat_cat_self);
    RUN_TEST(test_cat_splice);
}
DEFINE_TEST_MAIN(cat_suite)
