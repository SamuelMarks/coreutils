#include "test_helper.h"
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>





TEST test_echo_basic(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("env echo hello world", out, sizeof(out))); 
    trim_newline(out); 
    ASSERT_STR_EQ("hello world", out);
    PASS();
}

TEST test_echo_n(void) {
    char out[256];
    ASSERT_EQ(0, exec_capture("env echo -n hello", out, sizeof(out))); 
    trim_newline(out); 
    ASSERT_STR_EQ("hello", out);
    PASS();
}

TEST test_echo_escape(void) {
    char out[1024];

    // Note 4 octal digits are allowed
    ASSERT_EQ(0, exec_capture("env echo -n -e '\\x1b\\n\\e\\n\\33\\n\\033\\n\\0033\\n'", out, sizeof(out)));
    ASSERT_STR_EQ("\x1b\n\x1b\n\033\n\033\n\033\n", out);

    // Incomplete hex escapes are output as is
    ASSERT_EQ(0, exec_capture("env echo -n -e '\\x\\n'", out, sizeof(out)));
    ASSERT_STR_EQ("\\x\n", out);

    PASS();
}

TEST test_echo_dash_dash(void) {
    char out[1024];
    
    // Always output --
    ASSERT_EQ(0, exec_capture("env echo -- 'foo'", out, sizeof(out)));
    ASSERT_STR_EQ("-- foo\n", out);

    ASSERT_EQ(0, exec_capture("env echo -n -e -- 'foo\\n'", out, sizeof(out)));
    ASSERT_STR_EQ("-- foo\n", out);

    PASS();
}

TEST test_echo_c_stop(void) {
    char out[1024];
    // Ensure \c stops processing
    ASSERT_EQ(0, exec_capture("env echo -e 'foo\\n\\cbar'", out, sizeof(out)));
    ASSERT_STR_EQ("foo\n", out);

    PASS();
}

TEST test_echo_posix(void) {
    char out[1024];
#ifndef _WIN32
    // With POSIXLY_CORRECT:
    // only -n as the first (separate) option enables option processing
    // -E is ignored
    // escapes are processed by default

    // setenv("POSIXLY_CORRECT", "1", 1);
    
    // Actually exec_capture doesn't take env vars easily except by prefixing or using setenv in the child.
    // Let's use `env POSIXLY_CORRECT=1`
    
    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 echo -n -E 'foo\\n'", out, sizeof(out)));
    ASSERT_STR_EQ("foo\n", out);

    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 echo -nE 'foo'", out, sizeof(out)));
    ASSERT_STR_EQ("-nE foo\n", out);

    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 echo -E -n 'foo'", out, sizeof(out)));
    ASSERT_STR_EQ("-E -n foo\n", out);

    ASSERT_EQ(0, exec_capture("env POSIXLY_CORRECT=1 echo --version", out, sizeof(out)));
    ASSERT_STR_EQ("--version\n", out);
#endif
    PASS();
}

TEST test_echo_other_escapes(void) {
    char out[1024];
    
    // Output a literal '-'
    ASSERT_EQ(0, exec_capture("env echo -", out, sizeof(out)));
    ASSERT_STR_EQ("-\n", out);

    // Output a literal backslash '\', no newline.
    ASSERT_EQ(0, exec_capture("env echo -n -e '\\\\'", out, sizeof(out)));
    ASSERT_STR_EQ("\\", out);

    // Test other characters escaped by a backslash
    ASSERT_EQ(0, exec_capture("env echo -n -e '\\a\\b\\e\\f\\n\\r\\t\\v'", out, sizeof(out)));
    ASSERT_STR_EQ("\a\b\x1b\f\n\r\t\v", out);

    // Output hex values which contain hexadecimal characters to test hextobin().
    // Hex values 4a through 4f are ASCII "JKLMNO".
    ASSERT_EQ(0, exec_capture("env echo -n -e '\\x4a\\x4b\\x4c\\x4d\\x4e\\x4f\\x4A\\x4B\\x4C\\x4D\\x4E\\x4F'", out, sizeof(out)));
    ASSERT_STR_EQ("JKLMNOJKLMNO", out);

    PASS();
}





SUITE(echo_suite) {
    RUN_TEST(test_echo_basic);
    RUN_TEST(test_echo_n);
    RUN_TEST(test_echo_escape);
    RUN_TEST(test_echo_dash_dash);
    RUN_TEST(test_echo_c_stop);
    RUN_TEST(test_echo_posix);
    RUN_TEST(test_echo_other_escapes);
}
DEFINE_TEST_MAIN(echo_suite)
