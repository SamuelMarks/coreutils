#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>



static void cleanup() {
    unlink("a");
    unlink("a~");
}

static void create_file(const char *name, const char *content) {
    FILE *f = fopen(name, "w");
    if (f) {
        fprintf(f, "%s", content);
        if (f) fclose(f);
    }
}

TEST test_cp_backup_is_src(void) {
    cleanup();

    create_file("a", "a\n");
    create_file("a~", "a-tilde\n");

    char out[1024];
    int res = exec_capture("cp --b=simple a~ a", out, sizeof(out));
    ASSERT_NEQ(0, res);

    // Normalize output to remove executable name and CR if present
    char normalized_out[1024] = {0};
    char *p = out;
    int j = 0;
    while (*p && j < sizeof(normalized_out) - 1) {
        if (*p != '\r') {
            normalized_out[j++] = *p;
        }
        p++;
    }
    normalized_out[j] = '\0';
    
    // Output should contain "backing up 'a' might destroy source;  'a~' not copied\n"
    const char *expected = "backing up 'a' might destroy source;  'a~' not copied\n";
    ASSERT(strstr(normalized_out, expected) != NULL);

    cleanup();
    PASS();
}





SUITE(cp_backup_is_src_suite) {
    RUN_TEST(test_cp_backup_is_src);
}
DEFINE_TEST_MAIN(cp_backup_is_src_suite)
