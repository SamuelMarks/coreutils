#include "test_helper.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>



static void cleanup() {
    my_system("cmake -E rm -rf a b c d old new yes.txt no.txt empty.txt");
}

static void create_file(const char *name, const char *content) {
    FILE *f = fopen(name, "w");
    if (f) {
        if (content) fprintf(f, "%s", content);
        if (f) fclose(f);
    }
}

#ifdef _WIN32
#define MKDIR(path) mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0700)
#endif

TEST test_cp_i(void) {
    cleanup();

    MKDIR("a");
    MKDIR("b");
    MKDIR("b/a");
    MKDIR("b/a/c");
    create_file("a/c", "");

    create_file("yes.txt", "y\n");
    create_file("no.txt", "n\n");

    // coreutils 6.2 cp would neglect to prompt in this case.
    ASSERT_NEQ(0, exec_capture("cp -iR a b < no.txt", NULL, 0));

    // test miscellaneous combinations of -f -i -n parameters
    create_file("c", "");
    create_file("d", "");

    char out[1024];

    // ask for overwrite, answer no
    ASSERT_NEQ(0, exec_capture("cp -vi c d < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") == NULL);

    // ask for overwrite, answer yes
    ASSERT_EQ(0, exec_capture("cp -vi c d < yes.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") != NULL);

    // -i wins over -n
    ASSERT_EQ(0, exec_capture("cp -vni c d < yes.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") != NULL);

    // -n wins over -i
    ASSERT_EQ(0, exec_capture("cp -vin c d < yes.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") == NULL);

    // -n wins over -i non verbose
    ASSERT_EQ(0, exec_capture("cp -in c d < yes.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") == NULL);

    // ask for overwrite, answer yes
    ASSERT_EQ(0, exec_capture("cp -vfi c d < yes.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") != NULL);

    // do not ask, prevent from overwrite
    ASSERT_EQ(0, exec_capture("cp -vfn c d < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") == NULL);

    // do not ask, prevent from overwrite
    ASSERT_EQ(0, exec_capture("cp -vnf c d < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "'c' -> 'd'") == NULL);

    // options --backup and --no-clobber are mutually exclusive
    ASSERT_NEQ(0, exec_capture("cp -bn c d", NULL, 0));
    
    // options --backup and --update=none{,-fail} are mutually exclusive
    ASSERT_NEQ(0, exec_capture("cp -b --update=none c d", NULL, 0));
    ASSERT_NEQ(0, exec_capture("cp -b --update=none-fail c d", NULL, 0));

    // Verify -i combines with -u,
    create_file("old", "old\n");
    sleep(1);
    create_file("new", "new\n");
    
    // coreutils 9.3 had --update={all,older} ignore -i
    ASSERT_NEQ(0, exec_capture("cp -vi --update=older new old < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);
    
    ASSERT_NEQ(0, exec_capture("cp -vi --update=all new old < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);
    
    // coreutils 9.5 also had -u ignore -i
    ASSERT_NEQ(0, exec_capture("cp -vi -u new old < no.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);
    
    // Don't prompt as not updating
    create_file("empty.txt", "");
    ASSERT_EQ(0, exec_capture("cp -v -i --update=none new old < empty.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);
    
    ASSERT_EQ(0, exec_capture("cp -v --update=none -i new old < empty.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);
    
    ASSERT_EQ(0, exec_capture("cp -v -n --update=none -i new old < empty.txt", out, sizeof(out)));
    ASSERT(strstr(out, "->") == NULL);

    cleanup();
    PASS();
}





SUITE(cp_i_suite) {
    RUN_TEST(test_cp_i);
}
DEFINE_TEST_MAIN(cp_i_suite)
