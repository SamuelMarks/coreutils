#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>



static void cleanup() {
    my_system("rm -f f g");
}

static void create_file(const char *name) {
    FILE *f = fopen(name, "w");
    if (f) if (f) fclose(f);
}

TEST test_cp_preserve_2(void) {
    cleanup();

    create_file("f");

    ASSERT_EQ(0, exec_capture("cp --preserve=mode,links f g", NULL, 0));

    struct stat st;
    ASSERT_EQ(0, stat("g", &st));

    cleanup();
    PASS();
}





SUITE(cp_preserve_2_suite) {
    RUN_TEST(test_cp_preserve_2);
}
DEFINE_TEST_MAIN(cp_preserve_2_suite)
